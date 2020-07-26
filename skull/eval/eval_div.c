#include <stddef.h>

#include "eval_oper.h"

#include "eval_div.h"

/*
Make a new variable by dividing `rhs` from `lhs`.
*/
variable_t *eval_div(const variable_t *lhs, const variable_t *rhs) {
	if (lhs->type!=rhs->type) {
		return NULL;
	}

	if (lhs->type==&TYPE_INT) {
		variable_t *ret=make_variable(U"int", U"tmp", false);
		if (ret==NULL) {
			return NULL;
		}
		int64_t rhs_tmp=0;
		variable_read(&rhs_tmp, rhs);

		if (rhs_tmp==0) {
			return NULL;
		}

		int64_t lhs_tmp=0;
		variable_read(&lhs_tmp, lhs);

		const int64_t tmp=lhs_tmp / rhs_tmp;
		variable_write(ret, &tmp);

		return ret;
	}
	if (lhs->type==&TYPE_FLOAT) {
		DO_MATH(U"float", long double, /);
	}
	return NULL;
}