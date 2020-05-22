#pragma once

#include "variable.h"

/*
Make a new variable by adding the `lhs` and `rhs` together.
*/
variable_t *eval_add(const variable_t *lhs, const variable_t *rhs) {
	if (lhs->type!=rhs->type) {
		return NULL;
	}

	if (lhs->type==find_type(L"int")) {
		variable_t *ret=make_variable(L"int", L"", false);

		if (ret==NULL) {
			return NULL;
		}

		int64_t tmp=(*(int64_t*)lhs->mem) + (*(int64_t*)rhs->mem);

		memcpy(ret->mem, &tmp, sizeof(int64_t));
		return ret;
	}

	return NULL;
}