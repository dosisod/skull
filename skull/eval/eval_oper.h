#pragma once

#include "skull/eval/variable.h"

#define DO_MATH(str_type, real_type, oper) \
	Variable *const ret = make_variable(str_type, U"tmp", false); \
	if (!ret) { \
		return NULL; \
	} \
	real_type lhs_tmp = 0; \
	variable_read(&lhs_tmp, lhs); \
	real_type rhs_tmp = 0; \
	variable_read(&rhs_tmp, rhs); \
	const real_type tmp = lhs_tmp oper rhs_tmp; \
	variable_write(ret, &tmp); \
	return ret;

Variable *eval_oper(OperFunc func, const Variable *const, const Variable *const);
