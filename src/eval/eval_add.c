#include <stddef.h>
#include <string.h>

#include "eval_add.h"

variable_t *eval_add(const variable_t *lhs, const variable_t *rhs) {
	if (lhs->type!=rhs->type) {
		return NULL;
	}

	if (lhs->type==&TYPE_INT) {
		variable_t *ret=make_variable(U"int", U"tmp", false);

		if (ret==NULL) {
			return NULL;
		}

		int64_t lhs_tmp=0;
		variable_read(&lhs_tmp, lhs);

		int64_t rhs_tmp=0;
		variable_read(&rhs_tmp, rhs);

		const int64_t tmp=lhs_tmp + rhs_tmp;

		memcpy(ret->mem, &tmp, sizeof(int64_t));
		return ret;
	}

	if (lhs->type==&TYPE_FLOAT) {
		variable_t *ret=make_variable(U"float", U"tmp", false);

		if (ret==NULL) {
			return NULL;
		}

		long double lhs_tmp=0;
		variable_read(&lhs_tmp, lhs);

		long double rhs_tmp=0;
		variable_read(&rhs_tmp, rhs);

		const long double tmp=lhs_tmp + rhs_tmp;

		memcpy(ret->mem, &tmp, sizeof(long double));
		return ret;
	}

	return NULL;
}