#include <stddef.h>

#include "skull/eval/eval_add.h"

/*
Make a new variable by adding the `lhs` and `rhs` together.
*/
Variable *eval_add(const Variable *lhs, const Variable *rhs) {
	if (lhs->type != rhs->type) {
		return NULL;
	}
	if (lhs->type && lhs->type->add) {
		return lhs->type->add(lhs, rhs);
	}
	return NULL;
}