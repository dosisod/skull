#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/c/shared.h"
#include "skull/common/str.h"
#include "skull/semantic/func.h"
#include "skull/semantic/types.h"
#include "skull/semantic/variable.h"

#include "skull/codegen/c/expr.h"

static CExpr gen_expr_const_c(const AstNodeExpr *);
static CExpr gen_expr_binary_c(const AstNodeExpr *);
static CExpr gen_expr_unary_c(const AstNodeExpr *);

CExpr gen_expr_c(const AstNodeExpr *expr) {
	switch (expr->oper) {
		case EXPR_CONST:
			return gen_expr_const_c(expr);
		case EXPR_IDENTIFIER:
			return strdup(expr->var->name);
		case EXPR_ADD:
		case EXPR_SUB:
		case EXPR_MULT:
		case EXPR_DIV:
		case EXPR_MOD:
		case EXPR_LSHIFT:
		case EXPR_RSHIFT:
		case EXPR_IS:
		case EXPR_ISNT:
		case EXPR_LESS_THAN:
		case EXPR_GTR_THAN:
		case EXPR_LESS_THAN_EQ:
		case EXPR_GTR_THAN_EQ:
		case EXPR_AND:
		case EXPR_OR:
		case EXPR_XOR:
		case EXPR_POW:
			return gen_expr_binary_c(expr);
		case EXPR_UNARY_NEG:
		case EXPR_NOT:
		case EXPR_REF:
		case EXPR_DEREF:
			return gen_expr_unary_c(expr);
		case EXPR_FUNC:
			return gen_expr_func_call_c(expr->lhs.func_call);
		default:
			return NULL;
	}
}

static CExpr gen_expr_const_c(const AstNodeExpr *expr) {
	if (expr->type == &TYPE_INT) {
		const int64_t i = expr->value._int;

		// only add L suffix if value is larger then a C "int"
		if (i > INT_MAX || i < INT_MIN) {
			return uvsnprintf("%liL", i);
		}

		return uvsnprintf("%li", i);
	}
	if (expr->type == &TYPE_FLOAT) {
		if (!expr->value._int) return strdup("0.0");

		// here we are using the hex representation of the float to guarantee
		// that there is no loss in percision when stringifying.
		return uvsnprintf("%a", expr->value._float);
	}
	if (expr->type == &TYPE_BOOL) {
		return strdup(expr->value._bool ? "1" : "0");
	}
	if (expr->type == &TYPE_RUNE) {
		return uvsnprintf("0x%lX", expr->value.rune);
	}
	if (expr->type == &TYPE_STR) {
		return uvsnprintf("\"%s\"", expr->value.str);
	}

	return NULL;
}

static CExpr gen_expr_binary_c(const AstNodeExpr *expr) {
	const char *fmt = NULL;

	switch (expr->oper) {
		case EXPR_ADD: fmt = "(%s + %s)"; break;
		case EXPR_SUB: fmt = "(%s - %s)"; break;
		case EXPR_MULT: fmt = "(%s * %s)"; break;
		case EXPR_DIV: fmt = "(%s / %s)"; break;
		case EXPR_MOD: fmt = "(%s %% %s)"; break;
		case EXPR_LSHIFT: fmt = "(%s << %s)"; break;
		case EXPR_RSHIFT: fmt = "(%s >> %s)"; break;
		case EXPR_IS: {
			if (expr->lhs.expr->type == &TYPE_STR) {
				fmt = "_strcmp(%s, %s)";
				SKULL_STATE_C.called_strcmp = true;
			}
			else fmt = "(%s == %s)";
			break;
		}
		case EXPR_ISNT: {
			if (expr->lhs.expr->type == &TYPE_STR) {
				fmt = "!_strcmp(%s, %s)";
				SKULL_STATE_C.called_strcmp = true;
			}
			else fmt = "(%s != %s)";
			break;
		}
		case EXPR_LESS_THAN: fmt = "(%s < %s)"; break;
		case EXPR_GTR_THAN: fmt = "(%s > %s)"; break;
		case EXPR_LESS_THAN_EQ: fmt = "(%s <= %s)"; break;
		case EXPR_GTR_THAN_EQ: fmt = "(%s >= %s)"; break;
		case EXPR_AND: fmt = "(%s && %s)"; break;
		case EXPR_OR: fmt = "(%s || %s)"; break;
		case EXPR_XOR: fmt = "(%s ^ %s)"; break;
		case EXPR_POW: {
			if (expr->type == &TYPE_INT) {
				fmt = "_int_pow(%s, %s)";
				SKULL_STATE_C.called_int_pow = true;
			}
			else if (expr->type == &TYPE_FLOAT) {
				fmt = "_float_pow(%s, %s)";
				SKULL_STATE_C.called_float_pow = true;
			}
			break;
		}
		default: return NULL;
	}

	CExpr expr_lhs = gen_expr_c(expr->lhs.expr);
	CExpr expr_rhs = gen_expr_c(expr->rhs);

	CExpr out = uvsnprintf(fmt, expr_lhs, expr_rhs);

	free(expr_lhs);
	free(expr_rhs);
	return out;
}

static CExpr gen_expr_unary_c(const AstNodeExpr *expr) {
	const char *fmt = NULL;

	switch (expr->oper) {
		case EXPR_UNARY_NEG: fmt = "-(%s)"; break;
		case EXPR_NOT: fmt = "!%s"; break;
		case EXPR_REF: fmt = "&%s"; break;
		case EXPR_DEREF: fmt = "*%s"; break;
		default: break;
	}

	if (!fmt) return NULL;

	CExpr expr_str = gen_expr_c(expr->rhs);
	CExpr out = uvsnprintf(fmt, expr_str);

	free(expr_str);
	return out;
}

CExpr gen_expr_func_call_c(const AstNodeFunctionCall *func_call) {
	FunctionDeclaration *function = func_call->func_decl;
	char *name = function->name;
	unsigned short num_params = function->num_params;

	if (num_params == 0) return uvsnprintf("%s()", name);

	const AstNode *param = func_call->params;
	char *param_list = NULL;
	unsigned short at = 0;

	while (at < num_params) {
		CExpr expr = gen_expr_c(param->expr);

		if (at == 0) {
			param_list = expr;
		}
		else {
			char *old_list = param_list;
			param_list = uvsnprintf("%s, %s", old_list, expr);
			free(old_list);
			free(expr);
		}

		at++;
		param = param->next;
	}

	CExpr temp = uvsnprintf("%s(%s)", name, param_list);

	free(param_list);
	return temp;
}
