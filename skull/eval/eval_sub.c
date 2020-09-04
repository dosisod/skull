#include <stddef.h>

#include "skull/eval/eval_sub.h"

/*
Make a new variable by subtracting `rhs` from `lhs`.
*/
Variable *eval_sub(const Variable *lhs, const Variable *rhs) {
	if (lhs->type != rhs->type) {
		return NULL;
	}
	if (lhs->type && lhs->type->subtract) {
		return lhs->type->subtract(lhs, rhs);
	}
	return NULL;
}