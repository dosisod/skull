#include <stddef.h>

#include "skull/eval/eval_mult.h"

/*
Make a new variable by multiplying `lhs` and `rhs`.
*/
Variable *eval_mult(const Variable *lhs, const Variable *rhs) {
	if (lhs->type != rhs->type) {
		return NULL;
	}
	if (lhs->type && lhs->type->multiply) {
		return lhs->type->multiply(lhs, rhs);
	}
	return NULL;
}