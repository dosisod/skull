#include <stdlib.h>
#include <string.h>

#include "skull/semantic/types.h"
#include "skull/common/str.h"

#include "skull/codegen/c/expr.h"

char *expr_node_to_string(const AstNodeExpr *expr) {
	if (expr->type == TYPE_INT) {
		return uvsnprintf("%liL", expr->value._int);
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
