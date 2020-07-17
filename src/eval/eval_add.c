#include <stddef.h>

#include "../common/malloc.h"
#include "../common/str.h"

#include "eval_oper.h"

#include "eval_add.h"

/*
Make a new variable by adding the `lhs` and `rhs` together.
*/
variable_t *eval_add(const variable_t *lhs, const variable_t *rhs) {
	if (lhs->type!=rhs->type) {
		return NULL;
	}

	if (lhs->type==&TYPE_INT) {
		DO_MATH(U"int", int64_t, +);
	}
	if (lhs->type==&TYPE_FLOAT) {
		DO_MATH(U"float", long double, +);
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

		variable_write(ret, &cat);
		return ret;
	}
	return NULL;
}