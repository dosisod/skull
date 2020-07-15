#include <stddef.h>
#include <string.h>

#include "../common/malloc.h"
#include "../common/str.h"
#include "eval_sub.h"

#define MAKE_SUBBER(str_type, real_type) \
	variable_t *ret=make_variable(str_type, U"tmp", false); \
	if (ret==NULL) { \
		return NULL; \
	} \
	real_type lhs_tmp=0; \
	variable_read(&lhs_tmp, lhs); \
	real_type rhs_tmp=0; \
	variable_read(&rhs_tmp, rhs); \
	const real_type tmp=lhs_tmp - rhs_tmp; \
	memcpy(ret->mem, &tmp, sizeof(real_type)); \
	return ret;

/*
Make a new variable by subtracting `rhs` from `lhs`.
*/
variable_t *eval_sub(const variable_t *lhs, const variable_t *rhs) {
	if (lhs->type!=rhs->type) {
		return NULL;
	}

	if (lhs->type==&TYPE_INT) {
		MAKE_SUBBER(U"int", int64_t);
	}
	if (lhs->type==&TYPE_FLOAT) {
		MAKE_SUBBER(U"float", long double);
	}
	return NULL;
}

#undef MAKE_SUBBER