#include <stddef.h>

#include "skull/eval/eval_mult.h"

/*
Make a new variable by multiplying `lhs` and `rhs`.
*/
variable_t *eval_mult(const variable_t *lhs, const variable_t *rhs) {
	if (lhs->type!=rhs->type) {
		return NULL;
	}
	if (lhs->type && lhs->type->multiply) {
		return lhs->type->multiply(lhs, rhs);
	}
	return NULL;
}