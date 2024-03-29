#include <stdbool.h>

#include "skull/codegen/llvm/fwd_decl.h"
#include "skull/common/errors.h"
#include "skull/common/hashtable.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/symbol.h"
#include "skull/semantic/types.h"

#include "skull/semantic/expr.h"


static bool validate_stmt_func_call(SemanticState *, AstNodeExpr *);
static bool validate_ident_expr(SemanticState *, AstNodeExpr *);
static bool is_div_by_zero(const AstNodeExpr *);
static bool validate_const_expr(AstNodeExpr *);
static bool validate_pow_expr(const AstNodeExpr *);
static bool validate_bool_expr(const AstNodeExpr *);
static bool validate_ref_expr(const AstNodeExpr *);
static bool is_numeric(const AstNodeExpr *);
static bool validate_shift_expr(const AstNodeExpr *);
static Symbol *find_func_by_token(SemanticState *, const Token *);
static bool validate_func_call_params(SemanticState *, AstNodeFunctionCall *);
static void set_expr_type(SemanticState *, AstNodeExpr *);
static void set_const_expr(AstNodeExpr *);
static bool validate_binary_expr(SemanticState *, AstNodeExpr *);
static bool validate_expr_restrictions(AstNodeExpr *);
static bool is_binary_expr(ExprType);
static bool validate_deref_expr(const AstNodeExpr *);


bool validate_expr(SemanticState *state, AstNodeExpr *expr) {
	const ExprType oper = expr->oper;

	switch (oper) {
		case EXPR_FUNC: return validate_stmt_func_call(state, expr);
		case EXPR_IDENTIFIER: return validate_ident_expr(state, expr);
		case EXPR_CONST: return validate_const_expr(expr);
		default: break;
	}

	if (!validate_expr(state, expr->rhs)) return false;
	if (!validate_binary_expr(state, expr)) return false;
	if (!validate_expr_restrictions(expr)) return false;

	set_expr_type(state, expr);
	set_const_expr(expr);

	return true;
}

static bool validate_ident_expr(SemanticState *state, AstNodeExpr *expr) {
	Symbol *symbol = scope_find_var(state, expr->lhs.tok);
	if (!symbol || !symbol->var) return false;

	Variable *var = symbol->var;

	var->was_read = true;

	expr->symbol = symbol;
	expr->type = var->type;
	if (var->expr) expr->is_const_expr = var->expr->is_const_expr;

	return true;
}

static bool validate_binary_expr(SemanticState *state, AstNodeExpr *expr) {
	if (!is_binary_expr(expr->oper)) return true;

	if (!validate_expr(state, expr->lhs.expr)) return false;

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
		case EXPR_REF:
			return validate_ref_expr(expr);
		case EXPR_DEREF:
			return validate_deref_expr(expr);
		default: break;
	}

	return true;
}

static void set_expr_type(SemanticState *state, AstNodeExpr *expr) {
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
		case EXPR_XOR: expr->type = &TYPE_BOOL; break;
		case EXPR_REF: {
			expr->type = get_reference_type(state, expr->rhs->type);
			break;
		}
		case EXPR_DEREF: expr->type = expr->rhs->type->inner; break;
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
			expr->type = &TYPE_INT;
			break;
		}
		case TOKEN_FLOAT_CONST: {
			expr->value._float = eval_float(token, &err);
			expr->type = &TYPE_FLOAT;
			break;
		}
		case TOKEN_BOOL_CONST: {
			expr->value._bool = eval_bool(token);
			expr->type = &TYPE_BOOL;
			break;
		}
		case TOKEN_RUNE_CONST: {
			expr->value.rune = eval_rune(token, &err);
			expr->type = &TYPE_RUNE;
			break;
		}
		case TOKEN_STR_CONST: {
			char32_t *str = eval_str(token);
			if (!str) return false;

			char *const mbs = c32stombs(str, &token->location);

			free(str);
			if (!mbs) return false;

			expr->value.str = mbs;
			expr->type = &TYPE_STR;
			break;
		}
		default: return false;
	}

	expr->is_const_expr = true;
	return !err;
}

static bool validate_shift_expr(const AstNodeExpr *expr) {
	if (expr->rhs->type != &TYPE_INT) {
		FMT_ERROR(ERR_NOT_INT, { .loc = find_expr_node_location(expr) });

		return false;
	}

	return true;
}

static bool is_numeric(const AstNodeExpr *expr) {
	const bool ok = expr->rhs->type == &TYPE_INT ||
		expr->rhs->type == &TYPE_FLOAT;

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
	const Type *type = expr->rhs->type;

	if (type != &TYPE_INT && type != &TYPE_FLOAT) {
		FMT_ERROR(ERR_POW_BAD_TYPE, { .type = expr->rhs->type });

		return false;
	}

	return true;
}

static bool validate_bool_expr(const AstNodeExpr *expr) {
	if (expr->rhs->type != &TYPE_BOOL) {
		FMT_ERROR(ERR_EXPECTED_SAME_TYPE,
			{
				.loc = find_expr_node_location(expr),
				.type = &TYPE_BOOL
			},
			{ .type = expr->rhs->type }
		);

		return false;
	}
	return true;
}

static bool validate_ref_expr(const AstNodeExpr *expr) {
	if (expr->rhs->oper != EXPR_IDENTIFIER) {
		FMT_ERROR(ERR_REF_IDENT_ONLY, {
			.loc = find_expr_node_location(expr)
		});

		return false;
	}

	return true;
}

static bool validate_deref_expr(const AstNodeExpr *expr) {
	if (expr->rhs->oper != EXPR_IDENTIFIER || !is_reference(expr->rhs->type)) {
		FMT_ERROR(ERR_CANNOT_DEREF, { .loc = find_expr_node_location(expr) });

		return false;
	}

	return true;
}

bool validate_expr_func(SemanticState *state, const AstNode *node) {
	if (node->expr->oper == EXPR_FUNC)
		return validate_stmt_func_call(state, node->expr);

	FMT_ERROR(ERR_NO_DANGLING_EXPR, { .loc = &node->token->location });
	return false;
}

static bool validate_stmt_func_call(SemanticState *state, AstNodeExpr *expr) {
	AstNodeFunctionCall *func_call = expr->lhs.func_call;
	const Token *func_name_token = func_call->func_name_tok;

	Symbol *symbol = find_func_by_token(state, func_name_token);
	if (!symbol || !symbol->func) return false;

	FunctionDeclaration *function = symbol->func;

	func_call->symbol = symbol;
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

	if (num_params) return validate_func_call_params(state, func_call);

	return true;
}

static Symbol *find_func_by_token(SemanticState *state, const Token *token) {
	char *func_name = token_to_mbs_str(token);

	Symbol *symbol = find_func_by_name(state, func_name);
	free(func_name);

	if (symbol) return symbol;

	FMT_ERROR(ERR_MISSING_DECLARATION, { .tok = token });
	return NULL;
}

static bool validate_func_call_params(
	SemanticState *state,
	AstNodeFunctionCall *func_call
) {
	const FunctionDeclaration *function = func_call->symbol->func;
	const AstNode *param = func_call->params;

	for (unsigned i = 0; i < function->num_params; i++) {
		if (!validate_expr(state, param->expr)) return false;

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
	return !(
		oper == EXPR_UNARY_NEG ||
		oper == EXPR_NOT ||
		oper == EXPR_REF ||
		oper == EXPR_DEREF
	);
}
