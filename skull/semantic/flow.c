#include <stdbool.h>
#include <stdlib.h>

#include "skull/common/errors.h"
#include "skull/semantic/expr.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/symbol.h"

#include "skull/semantic/flow.h"


static bool validate_control_not_missing_if(const AstNode *);
static bool is_missing_block(const AstNode *, const char *);
static bool validate_bool_expr(const AstNodeExpr *);
static bool no_dead_code_below(const AstNode *);
static bool is_valid_return_expr(const AstNode *);

bool validate_stmt_return(const AstNode *node) {
	if (!assert_sane_child(node->next)) return false;

	if (node->expr && !validate_expr(node->expr_node)) return false;

	Type return_type = SEMANTIC_STATE.current_func->return_type;

	if (!node->expr_node && return_type != TYPE_VOID) {
		FMT_ERROR(ERR_RETURN_MISSING_EXPR, {
			.loc = &node->token->location
		});

		return false;
	}

	if (node->expr_node && !is_valid_return_expr(node)) return false;

	return no_dead_code_below(node->next);
}

static bool is_valid_return_expr(const AstNode *node) {
	const bool is_main = \
		SEMANTIC_STATE.current_func == SEMANTIC_STATE.main_func;

	const AstNodeExpr *expr = node->expr_node->expr;

	if (is_main && expr->type != TYPE_INT) {
		FMT_ERROR(ERR_NON_INT_MAIN, { .tok = node->expr_node->token });

		return false;
	}

	Type return_type = SEMANTIC_STATE.current_func->return_type;

	if (return_type != TYPE_VOID && expr->type != return_type) {
		FMT_ERROR(ERR_EXPECTED_SAME_TYPE,
			{ .loc = &node->expr_node->token->location, .type = return_type },
			{ .type = expr->type }
		);

		return false;
	}

	return true;
}

bool validate_stmt_unreachable(const AstNode *node) {
	return no_dead_code_below(node->next);
}

static bool no_dead_code_below(const AstNode *node) {
	while (node) {
		switch (node->type) {
			case AST_NODE_COMMENT:
			case AST_NODE_UNREACHABLE:
			case AST_NODE_NOOP:
				break;
			default: {
				FMT_ERROR(ERR_UNREACHABLE_CODE, {
					.loc = &node->token->location
				});

				return false;
			}
		}

		node = node->next;
	}

	return true;
}

bool validate_control_else(const AstNode *node) {
	return validate_control_not_missing_if(node) &&
		!is_missing_block(node, "else");
}

bool validate_control_if(const AstNode *node) {
	return !is_missing_block(node, "if") &&
		validate_expr(node->expr_node) &&
		validate_bool_expr(node->expr_node->expr);
}

bool validate_control_elif(const AstNode *node) {
	return !is_missing_block(node, "elif") &&
		validate_control_not_missing_if(node) &&
		validate_expr(node->expr_node) &&
		validate_bool_expr(node->expr_node->expr);
}

bool validate_control_while(const AstNode *node) {
	if (is_missing_block(node, "while") ||
		!validate_expr(node->expr_node) ||
		!validate_bool_expr(node->expr_node->expr)
	) {
		return false;
	}

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

static bool is_missing_block(const AstNode *node, const char *name) {
	if (node->child) return false;

	FMT_ERROR(ERR_MISSING_BLOCK, {
		.loc = &node->token->location,
		.str = name
	});

	return true;
}

bool validate_control_not_missing_if(const AstNode *node) {
	const AstNode *last = node->last;

	if (last) {
		switch (last->type) {
			case AST_NODE_COMMENT:
				return validate_control_not_missing_if(last);
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

static bool validate_bool_expr(const AstNodeExpr *expr) {
	if (expr->type == TYPE_BOOL) return true;

	FMT_ERROR(ERR_NON_BOOL_EXPR, { .loc = find_expr_node_location(expr) });

	return false;
}

/*
Verify that `node` doens't contain child node if it shouldn't.

Return `true` if node is "sane".
*/
bool assert_sane_child(const AstNode *node) {
	if (!node) return true;

	const NodeType node_type = node->type;

	if (node->child && !(
		node_type == AST_NODE_IF ||
		node_type == AST_NODE_ELIF ||
		node_type == AST_NODE_ELSE ||
		node_type == AST_NODE_WHILE ||
		node_type == AST_NODE_FUNCTION_PROTO ||
		(node_type == AST_NODE_EXPR && node->expr->oper == EXPR_FUNC)
	)) {
		FMT_ERROR(ERR_UNEXPECTED_CODE_BLOCK, {
			.loc = &node->child->token->location
		});

		return false;
	}

	return true;
}
