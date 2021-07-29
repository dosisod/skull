#include <stdbool.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/symbol.h"
#include "skull/semantic/types.h"
#include "skull/semantic/variable.h"

#include "skull/semantic/func.h"


static Type var_def_node_to_type(const AstNode *);
static Variable *node_to_var(const AstNode *const);
static Type func_get_type(const AstNode *, const AstNodeExpr *);
static bool is_expr_compatible_with_var(const AstNodeExpr *, const Variable *);
bool validate_expr(AstNode *);


bool validate_stmt_var_def(const AstNode *node) {
	Variable *var = node_to_var(node);
	if (!var) return false;

	node->var_def->var = var;

	const bool ok = validate_expr(node->var_def->expr_node);
	var->is_defined = true;

	if (!ok) return false;

	return is_expr_compatible_with_var(node->var_def->expr_node->expr, var);
}

bool validate_stmt_var_assign(const AstNode *node) {
	Variable *var = scope_find_var(node->token);
	if (!var) return false;

	node->var_assign->var = var;

	if (var->is_const) {
		FMT_ERROR(ERR_REASSIGN_CONST, {
			.tok = node->token
		});

		return false;
	}

	var->was_reassigned = true;

	if (!validate_expr(node->var_assign->expr_node)) return false;

	return is_expr_compatible_with_var(node->var_assign->expr_node->expr, var);
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
		char *const type_name = token_to_mbs_str(token->next);

		type = find_type(type_name);
		free(type_name);

		if (!type) {
			FMT_ERROR(ERR_TYPE_NOT_FOUND, { .tok = token->next });

			return NULL;
		}
	}

	char32_t *const name = token_to_string(token);

	Variable *var = make_variable(
		type,
		name,
		node->var_def->is_const
	);
	free(name);
	var->implicitly_typed = node->var_def->is_implicit;
	var->is_exported = node->var_def->is_exported;
	var->location = token->location;

	Symbol *symbol;
	symbol = Calloc(1, sizeof *symbol);

	*symbol = (Symbol){
		.name = var->name,
		.expr_type = var->type,
		.location = &var->location,
		.type = SYMBOL_VAR,
		.var = var
	};

	if (scope_add_symbol(symbol)) {
		return var;
	}

	var->name = NULL;
	variable_disable_warnings(var);
	free_variable(var);
	free(symbol);

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
			expr = expr->rhs;
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
			const Variable *var = scope_find_var(expr->lhs.tok);
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
	const Token *func_name_token = expr->lhs.func_call->func_name_tok;

	char *const func_name = token_to_mbs_str(func_name_token);

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
			.real = token_to_mbs_str(node->token)
		});

		// suppress errors
		function->was_called = true;
		return NULL;
	}

	return type;
}

static bool is_expr_compatible_with_var(
	const AstNodeExpr *expr,
	const Variable *var
) {
	if (expr->type != var->type) {
		FMT_ERROR(ERR_EXPECTED_SAME_TYPE,
			{
				.loc = find_expr_node_location(expr),
				.type = var->type
			},
			{ .type = expr->type }
		);
		return false;
	}

	return true;
}
