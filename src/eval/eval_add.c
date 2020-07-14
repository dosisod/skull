#include <stddef.h>
#include <string.h>

#include "../common/malloc.h"
#include "../common/str.h"
#include "eval_add.h"

#define MAKE_ADDER(str_type, real_type) \
	variable_t *ret=make_variable(str_type, U"tmp", false); \
	if (ret==NULL) { \
		return NULL; \
	} \
	real_type lhs_tmp=0; \
	variable_read(&lhs_tmp, lhs); \
	real_type rhs_tmp=0; \
	variable_read(&rhs_tmp, rhs); \
	const real_type tmp=lhs_tmp + rhs_tmp; \
	memcpy(ret->mem, &tmp, sizeof(real_type)); \
	return ret;

/*
Make a new variable by adding the `lhs` and `rhs` together.
*/
variable_t *eval_add(const variable_t *lhs, const variable_t *rhs) {
	if (lhs->type!=rhs->type) {
		return NULL;
	}

	if (lhs->type==&TYPE_INT) {
		MAKE_ADDER(U"int", int64_t);
	}
	if (lhs->type==&TYPE_FLOAT) {
		MAKE_ADDER(U"float", long double);
	}
	if (lhs->type==&TYPE_STR) {
		variable_t *ret=make_variable(U"str", U"tmp", false);
		if (ret==NULL) {
			return NULL;
		}

		char32_t *lhs_tmp=NULL;
		variable_read(&lhs_tmp, lhs);
		char32_t *rhs_tmp=NULL;
		variable_read(&rhs_tmp, rhs);

		char32_t *cat=c32scat(lhs_tmp, rhs_tmp);
		DIE_IF_MALLOC_FAILS(cat);

		memcpy(ret->mem, &cat, sizeof(char32_t*));
		return ret;
	}
	return NULL;
}

#undef MAKE_ADDER