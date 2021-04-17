#include <stdbool.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/codegen/aliases.h"
#include "skull/codegen/func.h"
#include "skull/codegen/scope.h"
#include "skull/codegen/shared.h"
#include "skull/codegen/types.h"
#include "skull/common/errors.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"

#include "skull/codegen/var.h"

Type var_def_node_to_type(const AstNode *);

/*
Make and add a variable from `node` to Skull state.
*/
Variable *node_to_var(const AstNode *const node) {
	const Token *token = node->attr.var_def->name_tok;
	Type type = NULL;

	if (node->attr.var_def->is_implicit) {
		type = var_def_node_to_type(node);
	}
	else {
		char *const type_name = token_mbs_str(token->next);

		type = find_type(type_name);
		free(type_name);

		if (!type) {
			PANIC(ERR_TYPE_NOT_FOUND, { .tok = token->next });
		}
	}

	char32_t *const name = token_str(token);

	Variable *var = make_variable(
		type,
		name,
		node->attr.var_def->is_const
	);

	if (scope_add_var(SKULL_STATE.scope, var)) {
		free(name);
		return var;
	}
	free_variable(var);

	PANIC(ERR_VAR_ALREADY_DEFINED, { .tok = token });
}

/*
Returns the left-most expr that is either a constant, variable, or function.
*/
const AstNodeExpr *leftmost_expr(const AstNodeExpr *expr) {
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
Return a variable type based on `node`.
*/
Type var_def_node_to_type(const AstNode *node) {
	TokenType token_type = node->next->token->type;

	if (node->next->type == AST_NODE_EXPR) {
		const AstNodeExpr *expr = node->next->attr.expr;

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
			return scope_find_var(expr->lhs.tok)->type;
		}
		else if (expr->oper == EXPR_FUNC) {
			const Token *func_name_token = expr->func_call->func_name_tok;

			char *const func_name = token_mbs_str(func_name_token);

			const FunctionDeclaration *const function = ht_get(
				SKULL_STATE.function_decls,
				func_name
			);
			free(func_name);

			if (!function) {
				PANIC(ERR_MISSING_DECLARATION, { .tok = func_name_token });
			}

			Type type = function->return_type;

			if (!type) {
				PANIC(ERR_NO_VOID_ASSIGN, {
					.tok = func_name_token,
					.real = token_mbs_str(node->token)
				});
			}

			return type;
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

	PANIC(ERR_INVALID_INPUT, { .tok = node->next->token });
}
