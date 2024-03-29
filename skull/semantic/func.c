#include <stdbool.h>
#include <string.h>

#include "skull/build_data.h"
#include "skull/common/errors.h"
#include "skull/common/hashtable.h"
#include "skull/common/malloc.h"
#include "skull/semantic/entry.h"
#include "skull/semantic/mangle.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/symbol.h"
#include "skull/semantic/types.h"
#include "skull/semantic/variable.h"

#include "skull/semantic/func.h"

static const Type *validate_return_type(SemanticState *, const Token *);
static bool validate_func_params(
	SemanticState *,
	const AstNode *,
	FunctionDeclaration *
);
static bool validate_stmt_func_decl_(SemanticState *, const AstNode *);
static bool post_validate_stmt_func_decl(SemanticState *, const AstNode *);

bool validate_stmt_func_decl(SemanticState *state, const AstNode *node) {
	const unsigned while_loop_depth = state->while_loop_depth;
	state->while_loop_depth = 0;

	const bool is_valid = (
		validate_stmt_func_decl_(state, node) &&
		setup_and_validate_ast_sub_tree(state, node->child) &&
		post_validate_stmt_func_decl(state, node)
	);

	state->while_loop_depth = while_loop_depth;

	return is_valid;
}

static bool validate_stmt_func_decl_(
	SemanticState *state,
	const AstNode *node
) {
	const bool is_external = node->func_proto->is_external;
	const bool is_export = node->func_proto->is_export;
	const Token *const func_name_token = node->func_proto->name_tok;

	if ((is_export || is_external) && !is_top_lvl_scope(state)) {
		FMT_ERROR(ERR_NO_NESTED, { .tok = func_name_token });
		return false;
	}

	const Type *return_type = validate_return_type(
		state,
		node->func_proto->return_type_token
	);
	if (!return_type) return false;

	FunctionDeclaration *func = Calloc(1, sizeof(FunctionDeclaration));
	*func = (FunctionDeclaration){
		.is_external = is_external,
		.is_export = is_export,
		.return_type = return_type,
		.num_params = node->func_proto->num_params,
		.params = node->func_proto->params
	};

	Symbol *symbol = Calloc(1, sizeof(Symbol));
	*symbol = (Symbol){
		.name = token_to_mbs_str(func_name_token),
		.expr_type = func->return_type,
		.location = func_name_token->location,
		.type = SYMBOL_FUNC,
		.func = func
	};
	symbol->linkage_name = is_export ?
		mangle_name(symbol->name) :
		symbol->name;

	state->last_func = state->current_func;
	state->current_func = symbol;

	if (!scope_add_symbol(state, symbol)) {
		free(func);
		free(symbol);
		return false;
	}

	node->func_proto->symbol = symbol;

	return validate_func_params(state, node, func);
}

static bool post_validate_stmt_func_decl(
	SemanticState *state,
	const AstNode *node
) {
	const Symbol *symbol = state->current_func;
	const FunctionDeclaration *func = symbol->func;

	if (func->is_external) return true;

	const AstNode *terminal_node = node->child;

	while (
		terminal_node &&
		terminal_node->type != AST_NODE_RETURN &&
		terminal_node->type != AST_NODE_UNREACHABLE
	) {
		terminal_node = terminal_node->next;
	}

	const bool returned_value = terminal_node &&
		terminal_node->type == AST_NODE_RETURN &&
		!!terminal_node->expr;

	if (!returned_value && func->return_type != &TYPE_VOID) {
		FMT_ERROR(ERR_EXPECTED_RETURN, { .real = strdup(symbol->name) });

		return false;
	}
	if (returned_value && func->return_type == &TYPE_VOID) {
		FMT_ERROR(ERR_NO_VOID_RETURN, { .real = strdup(symbol->name) });

		return false;
	}

	state->current_func = state->last_func;

	return true;
}

static bool validate_func_params(
	SemanticState *state,
	const AstNode *node,
	FunctionDeclaration *function
) {
	if (!function->num_params) return true;

	function->param_types = Calloc(function->num_params, sizeof(Type));

	make_child_scope(state);

	AstNodeFunctionParam **params = node->func_proto->params;

	for (unsigned i = 0; i < function->num_params; i++) {
		function->param_types[i] = token_to_type(state, params[i]->type_name);

		if (!function->param_types[i]) {
			FMT_ERROR(ERR_TYPE_NOT_FOUND, { .tok = params[i]->type_name });

			restore_parent_scope(state);
			return false;
		}

		Variable *const param_var = make_variable(
			function->param_types[i],
			node->func_proto->params[i]->param_name,
			true
		);

		param_var->linkage_name = param_var->name;
		param_var->is_defined = true;

		if (function->is_external) variable_disable_warnings(param_var);

		Symbol *symbol = Calloc(1, sizeof *symbol);

		*symbol = (Symbol){
			.name = param_var->name,
			.expr_type = param_var->type,
			.location = *function->params[i]->location,
			.type = SYMBOL_VAR,
			.var = param_var
		};

		if (!scope_add_var(state, symbol)) {
			FMT_ERROR(ERR_SHADOW_VAR, {
				.var = param_var,
				.loc = &symbol->location
			});

			variable_disable_warnings(param_var);
			free_variable(param_var);
			free(symbol);

			restore_parent_scope(state);
			return false;
		}

		function->params[i]->symbol = symbol;
	}
	restore_parent_scope(state);

	return true;
}

static const Type *validate_return_type(
	SemanticState *state,
	const Token *token
) {
	const Type *type = token ? token_to_type(state, token) : &TYPE_VOID;

	if (token && !type) {
		FMT_ERROR(ERR_TYPE_NOT_FOUND, { .tok = token });

		return NULL;
	}

	return type;
}

/*
Return function declaration (as Symbol) called `name`, or `NULL` if not found.
*/
Symbol *find_func_by_name(SemanticState *state, char *name) {
	Symbol *symbol = scope_find_name(state->scope, name);
	if (symbol && symbol->type == SYMBOL_FUNC) return symbol;

	return NULL;
}

void free_function_declaration(Symbol *symbol) {
	if (!symbol || !symbol->func) return;

	FunctionDeclaration *func = symbol->func;

	free(symbol->name);
	if (symbol->name != symbol->linkage_name) free(symbol->linkage_name);

	free(func->param_types);
	free(func);
	free(symbol);
}
