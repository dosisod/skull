#include <stddef.h>

#include "eval_oper.h"

#include "eval_sub.h"

/*
Make a new variable by subtracting `rhs` from `lhs`.
*/
variable_t *eval_sub(const variable_t *lhs, const variable_t *rhs) {
	if (lhs->type!=rhs->type) {
		return NULL;
	}

	if (lhs->type==&TYPE_INT) {
		DO_MATH(U"int", int64_t, -);
	}
	if (lhs->type==&TYPE_FLOAT) {
		DO_MATH(U"float", long double, -);
	}
	return NULL;
}