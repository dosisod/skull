#include <stddef.h>

#include "skull/eval/eval_add.h"

/*
Make a new variable by adding the `lhs` and `rhs` together.
*/
variable_t *eval_add(const variable_t *lhs, const variable_t *rhs) {
	if (lhs->type!=rhs->type) {
		return NULL;
	}
	if (lhs->type!=NULL && lhs->type->add!=NULL) {
		return lhs->type->add(lhs, rhs);
	}
	return NULL;
}