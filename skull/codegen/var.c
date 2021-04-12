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

const Type *var_def_node_to_type(const AstNode *);

/*
Make and add a variable from `node` to Skull state.
*/
Variable *node_to_var(const AstNode *const node) {
	const Token *token = node->attr.var_def->name_tok;
	const Type *type = NULL;

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
const Type *var_def_node_to_type(const AstNode *node) {
	TokenType token_type = node->next->token->type;
	const NodeType node_type = node->next->type;

	const AstNodeExpr *expr = NULL;

	if (node_type == AST_NODE_EXPR) {
		expr = node->next->attr.expr;

		const ExprType oper = expr->oper;

		if (oper == EXPR_NOT ||
			oper == EXPR_IS ||
			oper == EXPR_ISNT ||
			oper == EXPR_LESS_THAN ||
			oper == EXPR_GTR_THAN ||
			oper == EXPR_LESS_THAN_EQ ||
			oper == EXPR_GTR_THAN_EQ ||
			oper == EXPR_AND ||
			oper == EXPR_OR ||
			oper == EXPR_XOR
		) {
			return &TYPE_BOOL;
		}

		expr = leftmost_expr(expr);

		// Check if expr is a paren expr. We basically have to iterate
		// until we find a non-paren token. To fix this we have to
		// set the start token for the node based on the start of the
		// expr itself. XXX: this will fail if the expr in parens is
		// a function.
		if (token_type == TOKEN_PAREN_OPEN) {
			Token *iter = node->next->token;

			while (iter && iter->type == TOKEN_PAREN_OPEN)
				iter = iter->next;

			if (iter)
				token_type = iter->type;
		}

		else if (expr->oper == EXPR_CONST || expr->oper == EXPR_IDENTIFIER) {
			token_type = expr->lhs.tok->type;
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

			const Type *type = function->return_type;
			if (!type) {
				PANIC(ERR_NO_VOID_ASSIGN, {
					.tok = func_name_token,
					.real = token_mbs_str(node->token)
				});
			}

			return type;
		}
	}

	if (token_type == TOKEN_BOOL_CONST)
		return &TYPE_BOOL;

	if (token_type == TOKEN_INT_CONST)
		return &TYPE_INT;

	if (token_type == TOKEN_FLOAT_CONST)
		return &TYPE_FLOAT;

	if (token_type == TOKEN_RUNE_CONST)
		return &TYPE_RUNE;

	if (token_type == TOKEN_STR_CONST)
		return &TYPE_STR;

	if (token_type == TOKEN_IDENTIFIER && expr) {
		return scope_find_var(expr->lhs.tok)->type;
	}

	PANIC(ERR_INVALID_INPUT, { .tok = node->next->token });
}
