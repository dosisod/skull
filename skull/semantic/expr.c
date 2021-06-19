#include <stdbool.h>

#include "skull/common/errors.h"
#include "skull/codegen/func.h"

#include "skull/semantic/expr.h"


static bool validate_stmt_func_call(AstNodeFunctionCall *);

bool validate_expr(AstNode *node) {
	if (node->expr->oper == EXPR_FUNC)
		return validate_stmt_func_call(node->expr->func_call);

	return true;
}

bool validate_expr_func(AstNode *node) {
	if (node->expr->oper == EXPR_FUNC)
		return validate_stmt_func_call(node->expr->func_call);

	FMT_ERROR(ERR_NO_DANGLING_EXPR, { .loc = &node->token->location });
	return false;
}

static bool validate_stmt_func_call(AstNodeFunctionCall *func_call) {
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

	return true;
}
