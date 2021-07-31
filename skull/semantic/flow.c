#include <stdbool.h>
#include <stdlib.h>

#include "skull/common/errors.h"
#include "skull/semantic/expr.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/symbol.h"

#include "skull/semantic/flow.h"


static bool validate_control_not_missing_if(const AstNode *);
bool assert_sane_child(const AstNode *);

bool validate_stmt_return(const AstNode *node) {
	if (!assert_sane_child(node->next)) return false;

	if (node->expr) return validate_expr(node->expr_node);

	return true;
}

bool validate_control_else(const AstNode *node) {
	return validate_control_not_missing_if(node);
}

bool validate_control_if(const AstNode *node) {
	return validate_expr(node->expr_node);
}

bool validate_control_elif(const AstNode *node) {
	return validate_control_not_missing_if(node) &&
		validate_expr(node->expr_node);
}

bool validate_control_while(const AstNode *node) {
	if (!validate_expr(node->expr_node)) return false;

	const AstNodeExpr *expr = node->expr_node->expr;

	switch (expr->oper) {
		case EXPR_CONST: {
			if (expr->lhs.tok->type == TOKEN_BOOL_CONST) return true;
			break;
		}
		case EXPR_IDENTIFIER: {
			Variable *var = scope_find_var(expr->lhs.tok);
			if (!var) return false;

			if (var->type == TYPE_BOOL) return true;
			break;
		}
		case EXPR_FUNC: {
			const Token *token = expr->lhs.func_call->func_name_tok;
			char *name = token_to_mbs_str(token);
			FunctionDeclaration *func = find_func_by_name(name);
			free(name);

			if (!func) {
				FMT_ERROR(ERR_MISSING_DECLARATION, {
					.tok = token
				});

				return false;
			}

			if (func->return_type == TYPE_BOOL) return true;
			break;
		}
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
			return true;
		default: break;
	}

	FMT_ERROR(ERR_NON_BOOL_EXPR, { .loc = &node->expr_node->token->location });
	return false;
}

bool validate_control_not_missing_if(const AstNode *node) {
	const AstNode *last = node->last;

	if (last) {
		switch (last->type) {
			case AST_NODE_COMMENT: return validate_control_else(last);
			case AST_NODE_IF:
			case AST_NODE_ELIF:
				return true;
			default: break;
		}
	}

	FMT_ERROR(ERR_ELSE_ELIF_MISSING_IF, {
		.loc = &node->token->location
	});

	return false;
}
