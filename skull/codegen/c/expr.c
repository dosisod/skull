#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/semantic/types.h"
#include "skull/semantic/variable.h"

#include "skull/codegen/c/expr.h"

static char *const_expr_node_to_string(const AstNodeExpr *);
static char *binary_expr_to_string(const AstNodeExpr *);

char *expr_node_to_string(const AstNodeExpr *expr) {
	switch (expr->oper) {
		case EXPR_CONST:
			return const_expr_node_to_string(expr);
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
			return binary_expr_to_string(expr);
		default:
			return NULL;
	}
}

static char *const_expr_node_to_string(const AstNodeExpr *expr) {
	if (expr->type == TYPE_INT) {
		const int64_t i = expr->value._int;

		// only add L suffix if value is larger then a C "int"
		if (i > INT_MAX || i < INT_MIN) {
			return uvsnprintf("%liL", i);
		}

		return uvsnprintf("%li", i);
	}
	if (expr->type == TYPE_FLOAT) {
		if (!expr->value._int) return strdup("0.0");

		// here we are using the hex representation of the float to guarantee
		// that there is no loss in percision when stringifying.
		return uvsnprintf("%a", expr->value._float);
	}
	if (expr->type == TYPE_BOOL) {
		return strdup(expr->value._bool ? "1" : "0");
	}
	if (expr->type == TYPE_RUNE) {
		return uvsnprintf("0x%lX", expr->value.rune);
	}

	return NULL;
}

static char *binary_expr_to_string(const AstNodeExpr *expr) {
	const char *fmt = NULL;

	switch (expr->oper) {
		case EXPR_ADD: fmt = "(%s + %s)"; break;
		case EXPR_SUB: fmt = "(%s - %s)"; break;
		case EXPR_MULT: fmt = "(%s * %s)"; break;
		case EXPR_DIV: fmt = "(%s / %s)"; break;
		case EXPR_MOD: fmt = "(%s %% %s)"; break;
		case EXPR_LSHIFT: fmt = "(%s << %s)"; break;
		case EXPR_RSHIFT: fmt = "(%s >> %s)"; break;
		case EXPR_IS: fmt = "(%s == %s)"; break;
		case EXPR_ISNT: fmt = "(%s != %s)"; break;
		case EXPR_LESS_THAN: fmt = "(%s < %s)"; break;
		case EXPR_GTR_THAN: fmt = "(%s > %s)"; break;
		case EXPR_LESS_THAN_EQ: fmt = "(%s <= %s)"; break;
		case EXPR_GTR_THAN_EQ: fmt = "(%s >= %s)"; break;
		case EXPR_AND: fmt = "(%s && %s)"; break;
		case EXPR_OR: fmt = "(%s || %s)"; break;
		case EXPR_XOR: fmt = "(%s ^ %s)"; break;
		case EXPR_POW: {
			if (expr->type == TYPE_INT) fmt = "_int_pow(%s, %s)";
			else if (expr->type == TYPE_FLOAT) fmt = "_float_pow(%s, %s)";
			break;
		}
		default: return NULL;
	}

	char *expr_lhs = expr_node_to_string(expr->lhs.expr);
	char *expr_rhs = expr_node_to_string(expr->rhs);

	char *out = uvsnprintf(fmt, expr_lhs, expr_rhs);

	free(expr_lhs);
	free(expr_rhs);
	return out;
}
