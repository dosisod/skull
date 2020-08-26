#include <stddef.h>

#include "skull/eval/eval_sub.h"

/*
Make a new variable by subtracting `rhs` from `lhs`.
*/
variable_t *eval_sub(const variable_t *lhs, const variable_t *rhs) {
	if (lhs->type != rhs->type) {
		return NULL;
	}
	if (lhs->type && lhs->type->subtract) {
		return lhs->type->subtract(lhs, rhs);
	}
	return NULL;
}