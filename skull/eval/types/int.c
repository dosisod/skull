#include "skull/common/str.h"
#include "skull/eval/eval_oper.h"
#include "skull/eval/types/sprint_fmt.h"

#include "skull/eval/types/int.h"

/*
Returns the string representation of int `var`
*/
char32_t *fmt_int_type(const variable_t *var) {
	int64_t data=0;
	variable_read(&data, var);

	SPRINTF_FMT("%li");

	char32_t *ret=mbstoc32s(tmp);
	free(tmp);
	return ret;
}

/*
Add `lhs` and `rhs` ints together
*/
variable_t *add_int_type(const variable_t *lhs, const variable_t *rhs) {
	DO_MATH(U"int", int64_t, +);
}

/*
Subtract `rhs` int from `lhs` int
*/
variable_t *sub_int_type(const variable_t *lhs, const variable_t *rhs) {
	DO_MATH(U"int", int64_t, -);
}

/*
Divide `lhs` int by `rhs` int

If `rhs` is zero, return NULL.
*/
variable_t *div_int_type(const variable_t *lhs, const variable_t *rhs) {
	variable_t *ret=make_variable(U"int", U"tmp", false);
	if (!ret) {
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

/*
Multiply `lhs` and `rhs` ints together
*/
variable_t *mult_int_type(const variable_t *lhs, const variable_t *rhs) {
	DO_MATH(U"int", int64_t, *);
}
