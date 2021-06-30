#include <stdbool.h>

#include "skull/codegen/llvm/fwd_decl.h"
#include "skull/common/errors.h"
#include "skull/common/hashtable.h"
#include "skull/semantic/func.h"

#include "skull/semantic/expr.h"


static bool validate_stmt_func_call(const AstNodeFunctionCall *);
static bool _validate_expr(const AstNodeExpr *);

bool validate_expr(const AstNode *node) {
	return _validate_expr(node->expr);
}

static bool _validate_expr(const AstNodeExpr *expr) {
	const ExprType oper = expr->oper;

	if (oper == EXPR_FUNC)
		return validate_stmt_func_call(expr->lhs.func_call);

	switch (oper) {
		case EXPR_UNKNOWN:
		case EXPR_IDENTIFIER:
		case EXPR_CONST:
			// these are opers that cannot have a nested expr
			return true;
		default: break;
	}

	const bool is_binary = !(oper == EXPR_UNARY_NEG || oper == EXPR_NOT);

	if (!_validate_expr(expr->rhs.expr)) return false;
	if (is_binary && !_validate_expr(expr->lhs.expr)) return false;

	return true;
}

bool validate_expr_func(const AstNode *node) {
	if (node->expr->oper == EXPR_FUNC)
		return validate_stmt_func_call(node->expr->lhs.func_call);

	FMT_ERROR(ERR_NO_DANGLING_EXPR, { .loc = &node->token->location });
	return false;
}

static bool validate_stmt_func_call(const AstNodeFunctionCall *func_call) {
	const Token *func_name_token = func_call->func_name_tok;
	char *const func_name = token_mbs_str(func_name_token);

	FunctionDeclaration *function = find_func_by_name(func_name);
	free(func_name);

	if (!function) {
		FMT_ERROR(ERR_MISSING_DECLARATION, {
			.tok = func_name_token
		});

		return false;
	}

	function->was_called = true;

	unsigned short num_params = function->num_params;

	if (num_params != func_call->num_values) {
		FMT_ERROR(ERR_INVALID_NUM_PARAMS, {
			.loc = &func_name_token->location
		});

		return false;
	}

	const AstNode *param = func_call->params;

	if (num_params == 0 && param->token) {
		FMT_ERROR(ERR_ZERO_PARAM_FUNC, { .loc = &param->token->location });

		return false;
	}

	if (num_params) {
		while (num_params) {
			validate_expr(param);
			param = param->next;
			num_params--;
		}
	}

	return true;
}
