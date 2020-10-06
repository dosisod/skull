#include <stddef.h>

#include "skull/eval/eval_oper.h"

/*
Handle operation `func` on 2 variables, `lhs` and `rhs`.

Returns new variable resulting from operation, or `NULL` on error.
*/
Variable *eval_oper(OperFunc func, const Variable *const lhs, const Variable *const rhs) {
	if (lhs->type != rhs->type) {
		return NULL;
	}
	if (func) {
		return func(lhs, rhs);
	}
	return NULL;
}
