#include <stdbool.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/hashtable.h"
#include "skull/common/malloc.h"
#include "skull/common/range.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/symbol.h"
#include "skull/semantic/types.h"
#include "skull/semantic/variable.h"

#include "skull/semantic/func.h"

static Type validate_return_type(char *);
static bool validate_func_params(const AstNode *, FunctionDeclaration *);

bool validate_stmt_func_decl(const AstNode *node) {
	const bool is_external = node->func_proto->is_external;
	const bool is_export = node->func_proto->is_export;
	const Token *const func_name_token = node->func_proto->name_tok;

	if ((is_export || is_external) && !is_top_lvl_scope()) {
		FMT_ERROR(ERR_NO_NESTED, { .tok = func_name_token });
		return false;
	}

	if (is_export && token_cmp(U"main", func_name_token)) {
		FMT_ERROR(ERR_MAIN_RESERVED, { .loc = &func_name_token->location });
		return false;
	}

	Type return_type = validate_return_type(node->func_proto->return_type_name);
	if (!return_type) return false;

	FunctionDeclaration *func = Calloc(1, sizeof(FunctionDeclaration));
	*func = (FunctionDeclaration){
		.name = token_to_mbs_str(func_name_token),
		.location = func_name_token->location,
		.is_external = is_external,
		.is_export = is_export,
		.return_type = return_type,
		.num_params = node->func_proto->num_params,
		.params = node->func_proto->params
	};
	SEMANTIC_STATE.last_func = SEMANTIC_STATE.current_func;
	SEMANTIC_STATE.current_func = func;

	node->func_proto->func = func;

	Symbol *symbol = Calloc(1, sizeof(Symbol));
	*symbol = (Symbol){
		.name = func->name,
		.expr_type = func->return_type,
		.location = &func->location,
		.type = SYMBOL_FUNC,
		.func = func
	};

	if (!scope_add_symbol(symbol)) {
		free(func);
		free(symbol);
		return false;
	}

	return validate_func_params(node, func);
}

bool post_validate_stmt_func_decl(const AstNode *node) {
	const FunctionDeclaration *func = SEMANTIC_STATE.current_func;

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

	if (!returned_value && func->return_type != TYPE_VOID) {
		FMT_ERROR(ERR_EXPECTED_RETURN, { .real = strdup(func->name) });

		return false;
	}
	if (returned_value && func->return_type == TYPE_VOID) {
		FMT_ERROR(ERR_NO_VOID_RETURN, { .real = strdup(func->name) });

		return false;
	}

	SEMANTIC_STATE.current_func = SEMANTIC_STATE.last_func;

	return true;
}

static bool validate_func_params(
	const AstNode *node,
	FunctionDeclaration *function
) {
	AstNodeFunctionParam **params = node->func_proto->params;
	if (!params) return true;

	function->param_types = Calloc(function->num_params, sizeof(Type));

	for RANGE(i, function->num_params) {
		function->param_types[i] = find_type(params[i]->type_name);

		if (!function->param_types[i]) {
			FMT_ERROR(ERR_TYPE_NOT_FOUND, { .str = params[i]->type_name });

			return false;
		}

		Variable *const param_var = make_variable(
			function->param_types[i],
			node->func_proto->params[i]->param_name,
			true
		);

		Location param_location = function->params[i]->var->location;
		free(function->params[i]->var);
		function->params[i]->var = NULL;
		param_var->location = param_location;
		param_var->is_defined = true;

		if (function->is_external) variable_disable_warnings(param_var);

		Symbol *symbol = Calloc(1, sizeof *symbol);

		*symbol = (Symbol){
			.name = param_var->name,
			.expr_type = param_var->type,
			.location = &param_var->location,
			.type = SYMBOL_VAR,
			.var = param_var
		};

		if (!scope_add_var(symbol)) {
			FMT_ERROR(ERR_SHADOW_VAR, {
				.var = param_var,
				.loc = &param_var->location
			});

			variable_disable_warnings(param_var);
			free_variable(param_var);
			free(symbol);

			return false;
		}

		function->params[i]->var = param_var;
	}

	return true;
}

static Type validate_return_type(char *type_name) {
	Type type = type_name ? find_type(type_name) : TYPE_VOID;

	if (type_name && !type) {
		FMT_ERROR(ERR_TYPE_NOT_FOUND, { .str = type_name });

		return NULL;
	}

	return type;
}

/*
Return function declaration called `name`, or `NULL` if not found.
*/
FunctionDeclaration *find_func_by_name(const char *name) {
	Symbol *symbol = scope_find_name(SEMANTIC_STATE.scope, name);
	if (symbol) return symbol->func;

	return NULL;
}

void free_function_declaration(FunctionDeclaration *func) {
	if (!func) return;

	if (!func->was_called) {
		FMT_WARN(WARN_FUNC_UNUSED, {
			.real = func->name, .loc = &func->location
		});
	}
	else free(func->name);

	free(func->param_types);
	free(func);
}
