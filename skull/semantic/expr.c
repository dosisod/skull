#include <stdbool.h>

#include "skull/codegen/llvm/fwd_decl.h"
#include "skull/common/errors.h"
#include "skull/common/hashtable.h"
#include "skull/common/range.h"
#include "skull/common/str.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"

#include "skull/semantic/expr.h"


static bool validate_stmt_func_call(AstNodeExpr *);
static bool validate_ident_expr(AstNodeExpr *);
static bool is_div_by_zero(const AstNodeExpr *);
static bool validate_const_expr(AstNodeExpr *);
static bool validate_pow_expr(const AstNodeExpr *);
static bool validate_bool_expr(const AstNodeExpr *);
static bool is_numeric(const AstNodeExpr *);
static bool validate_shift_expr(const AstNodeExpr *);
static FunctionDeclaration *find_func_by_token(const Token *);
static bool validate_func_call_params(AstNodeFunctionCall *);
static void set_expr_type(AstNodeExpr *);
static void set_const_expr(AstNodeExpr *);
static bool validate_binary_expr(AstNodeExpr *);
static bool validate_expr_restrictions(AstNodeExpr *);
static bool is_binary_expr(ExprType);


bool validate_expr(AstNodeExpr *expr) {
	const ExprType oper = expr->oper;

	switch (oper) {
		case EXPR_FUNC: return validate_stmt_func_call(expr);
		case EXPR_IDENTIFIER: return validate_ident_expr(expr);
		case EXPR_CONST: return validate_const_expr(expr);
		default: break;
	}

	if (!validate_expr(expr->rhs)) return false;
	if (!validate_binary_expr(expr)) return false;
	if (!validate_expr_restrictions(expr)) return false;

	set_expr_type(expr);
	set_const_expr(expr);

	return true;
}

static bool validate_ident_expr(AstNodeExpr *expr) {
	Variable *var = scope_find_var(expr->lhs.tok);
	if (!var) return false;

	var->was_read = true;

	expr->var = var;
	expr->type = var->type;
	if (var->expr) expr->is_const_expr = var->expr->is_const_expr;

	return true;
}

static bool validate_binary_expr(AstNodeExpr *expr) {
	if (!is_binary_expr(expr->oper)) return true;

	if (!validate_expr(expr->lhs.expr)) return false;

	if (expr->lhs.expr->type != expr->rhs->type) {
		FMT_ERROR(ERR_EXPECTED_SAME_TYPE,
			{
				.loc = find_expr_node_location(expr->rhs),
				.type = expr->lhs.expr->type
			},
			{ .type = expr->rhs->type }
		);
		return false;
	}

	return true;
}

static bool validate_expr_restrictions(AstNodeExpr *expr) {
	switch (expr->oper) {
		case EXPR_ADD:
		case EXPR_SUB:
		case EXPR_MULT:
		case EXPR_UNARY_NEG:
		case EXPR_LESS_THAN:
		case EXPR_GTR_THAN:
		case EXPR_LESS_THAN_EQ:
		case EXPR_GTR_THAN_EQ:
			return is_numeric(expr);
		case EXPR_DIV:
		case EXPR_MOD:
			return !is_div_by_zero(expr) && is_numeric(expr);
		case EXPR_POW:
			return validate_pow_expr(expr) && is_numeric(expr);
		case EXPR_LSHIFT:
		case EXPR_RSHIFT:
			return validate_shift_expr(expr);
		case EXPR_NOT:
		case EXPR_AND:
		case EXPR_OR:
		case EXPR_XOR:
			return validate_bool_expr(expr);
		default: break;
	}

	return true;
}

static void set_expr_type(AstNodeExpr *expr) {
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
			expr->type = TYPE_BOOL; break;
		default: expr->type = expr->rhs->type; break;
	}
}

static void set_const_expr(AstNodeExpr *expr) {
	if (is_binary_expr(expr->oper)) {
		expr->is_const_expr = (
			expr->lhs.expr->is_const_expr && expr->rhs->is_const_expr
		);
	}
	else expr->is_const_expr = expr->rhs->is_const_expr;
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

	expr->is_const_expr = true;
	return !err;
}

static bool validate_shift_expr(const AstNodeExpr *expr) {
	if (expr->rhs->type != TYPE_INT) {
		FMT_ERROR(ERR_NOT_INT, { .loc = find_expr_node_location(expr) });

		return false;
	}

	return true;
}

static bool is_numeric(const AstNodeExpr *expr) {
	const bool ok = expr->rhs->type == TYPE_INT ||
		expr->rhs->type == TYPE_FLOAT;

	if (!ok) {
		FMT_ERROR(ERR_NOT_NUMERIC, { .loc = find_expr_node_location(expr) });

		return false;
	}

	return true;
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

static bool validate_pow_expr(const AstNodeExpr *expr) {
	Type type = expr->rhs->type;

	if (type != TYPE_INT && type != TYPE_FLOAT) {
		FMT_ERROR(ERR_POW_BAD_TYPE, { .type = expr->rhs->type });

		return false;
	}

	return true;
}

static bool validate_bool_expr(const AstNodeExpr *expr) {
	if (expr->rhs->type != TYPE_BOOL) {
		FMT_ERROR(ERR_EXPECTED_SAME_TYPE,
			{
				.loc = find_expr_node_location(expr),
				.type = TYPE_BOOL
			},
			{ .type = expr->rhs->type }
		);

		return false;
	}
	return true;
}

bool validate_expr_func(const AstNode *node) {
	if (node->expr->oper == EXPR_FUNC)
		return validate_stmt_func_call(node->expr);

	FMT_ERROR(ERR_NO_DANGLING_EXPR, { .loc = &node->token->location });
	return false;
}

static bool validate_stmt_func_call(AstNodeExpr *expr) {
	AstNodeFunctionCall *func_call = expr->lhs.func_call;
	const Token *func_name_token = func_call->func_name_tok;

	FunctionDeclaration *function = find_func_by_token(func_name_token);
	if (!function) return false;

	func_call->func_decl = function;
	function->was_called = true;
	expr->type = function->return_type;

	unsigned short num_params = function->num_params;

	if (num_params != func_call->num_values) {
		FMT_ERROR(ERR_INVALID_NUM_PARAMS, {
			.loc = &func_name_token->location
		});

		return false;
	}

	const AstNode *param = func_call->params;

	if (num_params == 0 && param && param->token) {
		FMT_ERROR(ERR_ZERO_PARAM_FUNC, { .loc = &param->token->location });

		return false;
	}

	if (num_params) return validate_func_call_params(func_call);

	return true;
}

static FunctionDeclaration *find_func_by_token(const Token *token) {
	char *const func_name = token_to_mbs_str(token);

	FunctionDeclaration *function = find_func_by_name(func_name);
	free(func_name);

	if (function) return function;

	FMT_ERROR(ERR_MISSING_DECLARATION, { .tok = token });
	return NULL;
}

static bool validate_func_call_params(AstNodeFunctionCall *func_call) {
	const FunctionDeclaration *function = func_call->func_decl;
	const AstNode *param = func_call->params;

	for RANGE(i, function->num_params) {
		if (!validate_expr(param->expr)) return false;

		if (param->expr->type != function->param_types[i]) {
			FMT_ERROR(ERR_FUNC_TYPE_MISMATCH,
				{
					.loc = &param->token->location,
					.type = function->param_types[i]
				},
				{ .type = param->expr->type }
			);

			return false;
		}

		param = param->next;
	}

	return true;
}

static bool is_binary_expr(ExprType oper) {
	return !(oper == EXPR_UNARY_NEG || oper == EXPR_NOT);
}
