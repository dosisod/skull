#include <stddef.h>

#include "skull/eval/eval_div.h"

/*
Make a new variable by dividing `rhs` from `lhs`.
*/
Variable *eval_div(const Variable *lhs, const Variable *rhs) {
	if (lhs->type != rhs->type) {
		return NULL;
	}
	if (lhs->type && lhs->type->divide) {
		return lhs->type->divide(lhs, rhs);
	}
	return NULL;
}