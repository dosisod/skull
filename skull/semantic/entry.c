#include <stdbool.h>
#include <string.h>

#include "skull/codegen/scope.h"
#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/range.h"

#include "skull/semantic/entry.h"

static bool _validate_ast_tree(AstNode *);
static bool validate_ast_node(AstNode *);
static bool validate_stmt_return(AstNode *);
static bool validate_stmt_func_decl(AstNode *);
static bool validate_stmt_type_alias(AstNode *);
static bool validate_stmt_var_def(AstNode *);
bool assert_sane_child(AstNode *);
static void state_add_func(FunctionDeclaration *);

static Type var_def_node_to_type(const AstNode *);
static Variable *node_to_var(const AstNode *const);
static Type func_get_type(const AstNode *, const AstNodeExpr *);

/*
Validate an entire AST tree starting at `node`.
*/
bool validate_ast_tree(AstNode *node) {
	const bool pass = _validate_ast_tree(node);
	reset_scope_head();

	return pass;
}

/*
Validate an entire AST tree starting at `node`.
*/
static bool _validate_ast_tree(AstNode *node) {
	while (node) {
		if (node->child) {
			make_child_scope();
			const bool is_valid = _validate_ast_tree(node->child);
			restore_parent_scope();

			if (!is_valid) return false;
			make_adjacent_scope();
		}

		if (!validate_ast_node(node)) return false;
		node = node->next;
	}

	reset_scope_head();
	return true;
}

/*
Validate a single AST `node`.
*/
static bool validate_ast_node(AstNode *node) {
	if (node->type == AST_NODE_RETURN) {
		return validate_stmt_return(node);
	}
	if (node->type == AST_NODE_FUNCTION_PROTO) {
		return validate_stmt_func_decl(node);
	}
	if (node->type == AST_NODE_TYPE_ALIAS) {
		return validate_stmt_type_alias(node);
	}
	if (node->type == AST_NODE_VAR_DEF) {
		return validate_stmt_var_def(node);
	}

	return true;
}

static bool validate_stmt_return(AstNode *node) {
	return assert_sane_child(node->next);
}

static bool validate_stmt_func_decl(AstNode *node) {
	const bool is_external = node->func_proto->is_external;
	const bool is_export = node->func_proto->is_export;
	const Token *const func_name_token = node->func_proto->name_tok;

	if ((is_export || is_external) && !is_top_lvl_scope()) {
		FMT_ERROR(ERR_NO_NESTED, { .tok = func_name_token });
		return false;
	}

	char *func_name = token_mbs_str(func_name_token);

	if (is_export && strcmp(func_name, "main") == 0) {
		FMT_ERROR(ERR_MAIN_RESERVED, { .loc = &func_name_token->location });

		free(func_name);
		return false;
	}

	if (scope_find_name(SKULL_STATE.scope, func_name)) {
		FMT_ERROR(ERR_NO_REDEFINE_VAR_AS_FUNC, {
			.loc = &func_name_token->location,
			.real = func_name
		});

		return false;
	}

	FunctionDeclaration *found_func = find_func_by_name(func_name);

	if (found_func) {
		FMT_ERROR(ERR_NO_REDEFINE_FUNC, {
			.loc = &func_name_token->location,
			.real = func_name
		});

		return false;
	}

	Type return_type = NULL;
	char *return_type_name = node->func_proto->return_type_name;

	if (return_type_name)
		return_type = find_type(return_type_name);

	if (return_type_name && !return_type) {
		FMT_ERROR(ERR_TYPE_NOT_FOUND, { .str = return_type_name });

		free(func_name);
		return false;
	}

	FunctionDeclaration *func;
	func = Calloc(1, sizeof *func);
	func->name = func_name;
	func->location = func_name_token->location;
	func->is_external = is_external;
	func->is_export = is_export;
	func->return_type = return_type;
	func->param_names = node->func_proto->param_names;
	func->num_params = node->func_proto->num_params;

	node->func_proto->func = func;
	state_add_func(func);

	char **param_type_names = node->func_proto->param_type_names;
	if (!param_type_names) return true;

	func->param_types = Calloc(func->num_params, sizeof(Type));

	for RANGE(i, func->num_params) {
		func->param_types[i] = find_type(param_type_names[i]);

		if (!func->param_types[i]) {
			FMT_ERROR(ERR_TYPE_NOT_FOUND, { .str = param_type_names[i] });

			return false;
		}
	}

	return true;
}

static void state_add_func(FunctionDeclaration *func) {
	if (!SKULL_STATE.function_decls) {
		SKULL_STATE.function_decls = ht_create();
	}
	ht_add(SKULL_STATE.function_decls, func->name, func);
}

static bool validate_stmt_type_alias(AstNode *node) {
	const Token *const token = node->token;

	if (!is_top_lvl_scope()) {
		FMT_ERROR(ERR_TOP_LVL_ALIAS_ONLY, { .loc = &token->location });
		return false;
	}

	char *type_name = token_mbs_str(token->next->next);
	char *alias = token_mbs_str(token);

	const bool added = state_add_alias(find_type(type_name), alias);
	free(type_name);

	if (added) return true;

	FMT_ERROR(ERR_ALIAS_ALREADY_DEFINED, {
		.loc = &token->location,
		.real = alias
	});

	return false;
}

static bool validate_stmt_var_def(AstNode *node) {
	return !!node_to_var(node);
}

/*
Make and add a variable from `node` to Skull state.

Return `NULL` if an error occurred.
*/
static Variable *node_to_var(const AstNode *const node) {
	const Token *token = node->var_def->name_tok;
	Type type = NULL;

	if (node->var_def->is_implicit) {
		type = var_def_node_to_type(node);
		if (!type) return NULL;
	}
	else {
		char *const type_name = token_mbs_str(token->next);

		type = find_type(type_name);
		free(type_name);

		if (!type) {
			FMT_ERROR(ERR_TYPE_NOT_FOUND, { .tok = token->next });

			return NULL;
		}
	}

	char32_t *const name = token_str(token);

	Variable *var = make_variable(
		type,
		name,
		node->var_def->is_const
	);
	free(name);
	var->implicitly_typed = node->var_def->is_implicit;

	if (find_func_by_name(var->name)) {
		FMT_ERROR(ERR_NO_REDEFINE_FUNC_AS_VAR, {
			.loc = &token->location,
			.var = var
		});

		variable_no_warnings(var);
		free_variable(var);

		return NULL;
	}

	if (scope_add_var(&SKULL_STATE.scope, var)) {
		var->location = token->location;
		return var;
	}
	variable_no_warnings(var);
	free_variable(var);

	FMT_ERROR(ERR_VAR_ALREADY_DEFINED, { .tok = token });

	return NULL;
}

/*
Returns the left-most expr that is either a constant, variable, or function.
*/
static __attribute__((pure)) const AstNodeExpr *leftmost_expr(
	const AstNodeExpr *expr
) {
	while (expr->oper != EXPR_CONST &&
		expr->oper != EXPR_IDENTIFIER &&
		expr->oper != EXPR_FUNC
	) {
		if (expr->oper == EXPR_UNARY_NEG) {
			expr = expr->rhs.expr;
		}
		else {
			expr = expr->lhs.expr;
		}
	}

	return expr;
}

/*
Return a variable type based on `node`, `NULL` if an error occurred.
*/
static Type var_def_node_to_type(const AstNode *node) {
	AstNode *expr_node = node->var_def->expr_node;
	TokenType token_type = expr_node->token->type;

	if (expr_node->type == AST_NODE_EXPR) {
		const AstNodeExpr *expr = expr_node->expr;

		switch (expr->oper) {
			case EXPR_NOT:
			case EXPR_IS:
			case EXPR_ISNT:
			case EXPR_LESS_THAN:
			case EXPR_GTR_THAN:
			case EXPR_LESS_THAN_EQ:
			case EXPR_GTR_THAN_EQ:
			case EXPR_AND:
			case EXPR_OR:
			case EXPR_XOR:
				return TYPE_BOOL;
			default: break;
		}

		expr = leftmost_expr(expr);

		if (expr->oper == EXPR_CONST) {
			token_type = expr->lhs.tok->type;
		}
		else if (expr->oper == EXPR_IDENTIFIER) {
			const Variable *var = scope_find_var(expr->lhs.tok, true);
			if (!var) return NULL;

			return var->type;
		}
		else if (expr->oper == EXPR_FUNC) {
			return func_get_type(node, expr);
		}
	}

	switch (token_type) {
		case TOKEN_BOOL_CONST: return TYPE_BOOL;
		case TOKEN_INT_CONST: return TYPE_INT;
		case TOKEN_FLOAT_CONST: return TYPE_FLOAT;
		case TOKEN_RUNE_CONST: return TYPE_RUNE;
		case TOKEN_STR_CONST: return TYPE_STR;
		default: break;
	}

	FMT_ERROR(ERR_INVALID_INPUT, { .tok = node->next->token });

	return NULL;
}

static Type func_get_type(const AstNode *node, const AstNodeExpr *expr) {
	const Token *func_name_token = expr->func_call->func_name_tok;

	char *const func_name = token_mbs_str(func_name_token);

	FunctionDeclaration *const function = find_func_by_name(func_name);

	free(func_name);

	if (!function) {
		FMT_ERROR(ERR_MISSING_DECLARATION, { .tok = func_name_token });

		return NULL;
	}

	Type type = function->return_type;

	if (type == TYPE_VOID) {
		FMT_ERROR(ERR_NO_VOID_ASSIGN, {
			.loc = &func_name_token->location,
			.real = token_mbs_str(node->token)
		});

		// suppress errors
		function->was_called = true;
		return NULL;
	}

	return type;
}
