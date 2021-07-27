#include <stdbool.h>

#include "skull/codegen/llvm/fwd_decl.h"
#include "skull/common/errors.h"
#include "skull/common/hashtable.h"
#include "skull/common/str.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"

#include "skull/semantic/expr.h"


static bool validate_stmt_func_call(AstNodeFunctionCall *);
static bool _validate_expr(AstNodeExpr *);
static bool is_div_by_zero(const AstNodeExpr *);
static bool validate_const_expr(AstNodeExpr *);


bool validate_expr(const AstNode *node) {
	return _validate_expr(node->expr);
}

static bool _validate_expr(AstNodeExpr *expr) {
	const ExprType oper = expr->oper;

	switch (oper) {
		case EXPR_FUNC:
			return validate_stmt_func_call(expr->lhs.func_call);
		case EXPR_IDENTIFIER: {
			Variable *var = scope_find_var(expr->lhs.tok);
			expr->var = var;
			return !!var;
		}
		case EXPR_DIV:
		case EXPR_MOD: {
			if (is_div_by_zero(expr)) return false;
			break;
		}
		case EXPR_CONST: return validate_const_expr(expr);
		default: break;
	}

	if (!_validate_expr(expr->rhs)) return false;

	const bool is_binary = !(oper == EXPR_UNARY_NEG || oper == EXPR_NOT);
	if (is_binary && !_validate_expr(expr->lhs.expr)) return false;

	return true;
}

static bool validate_const_expr(AstNodeExpr *expr) {
	const Token *token = expr->lhs.tok;
	bool err = false;

	switch (token->type) {
		case TOKEN_INT_CONST: {
			expr->value._int = eval_integer(token, &err);
			expr->type = TYPE_INT;
			break;
		}
		case TOKEN_FLOAT_CONST: {
			expr->value._float = eval_float(token, &err);
			expr->type = TYPE_FLOAT;
			break;
		}
		case TOKEN_BOOL_CONST: {
			expr->value._bool = eval_bool(token);
			expr->type = TYPE_BOOL;
			break;
		}
		case TOKEN_RUNE_CONST: {
			expr->value.rune = eval_rune(token, &err);
			expr->type = TYPE_RUNE;
			break;
		}
		case TOKEN_STR_CONST: {
			char32_t *str = eval_str(token);
			if (!str) return false;

			char *const mbs = c32stombs(str, &token->location);

			free(str);
			if (!mbs) return false;

			expr->value.str = mbs;
			expr->type = TYPE_STR;
			break;
		}
		default: return false;
	}

	return !err;
}

static bool is_div_by_zero(const AstNodeExpr *expr) {
	if (expr->rhs->oper != EXPR_CONST) return false;

	const Token *token = expr->rhs->lhs.tok;
	if (token->type != TOKEN_INT_CONST) return false;

	bool err = false;
	const int64_t i = eval_integer(token, &err);
	if (err) return false;

	if (i == 0) {
		FMT_ERROR(ERR_DIV_BY_ZERO, { .loc = &token->location });

		return true;
	}

	return false;
}

bool validate_expr_func(const AstNode *node) {
	if (node->expr->oper == EXPR_FUNC)
		return validate_stmt_func_call(node->expr->lhs.func_call);

	FMT_ERROR(ERR_NO_DANGLING_EXPR, { .loc = &node->token->location });
	return false;
}

static bool validate_stmt_func_call(AstNodeFunctionCall *func_call) {
	const Token *func_name_token = func_call->func_name_tok;
	char *const func_name = token_to_mbs_str(func_name_token);

	FunctionDeclaration *function = find_func_by_name(func_name);
	free(func_name);

	if (!function) {
		FMT_ERROR(ERR_MISSING_DECLARATION, {
			.tok = func_name_token
		});

		return false;
	}

	func_call->func_decl = function;
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
			if (!validate_expr(param)) return false;
			param = param->next;
			num_params--;
		}
	}

	return true;
}
