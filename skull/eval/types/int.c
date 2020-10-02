#include "skull/common/str.h"
#include "skull/eval/eval_oper.h"
#include "skull/eval/types/defs.h"
#include "skull/eval/types/sprint_fmt.h"

#include "skull/eval/types/int.h"

/*
Returns the string representation of int `var`
*/
char32_t *fmt_int_type(const Variable *var) {
	SkullInt data = 0;
	variable_read(&data, var);

	SPRINTF_FMT("%li");

	char32_t *ret = mbstoc32s(tmp);
	free(tmp);
	return ret;
}

/*
Add `lhs` and `rhs` ints together
*/
Variable *add_int_type(const Variable *lhs, const Variable *rhs) {
	DO_MATH(&TYPE_INT, SkullInt, +);
}

/*
Subtract `rhs` int from `lhs` int
*/
Variable *sub_int_type(const Variable *lhs, const Variable *rhs) {
	DO_MATH(&TYPE_INT, SkullInt, -);
}

/*
Divide `lhs` int by `rhs` int

If `rhs` is zero, return NULL.
*/
Variable *div_int_type(const Variable *lhs, const Variable *rhs) {
	Variable *ret = make_variable(&TYPE_INT, U"tmp", false);
	if (!ret) {
		return NULL;
	}
	SkullInt rhs_tmp = 0;
	variable_read(&rhs_tmp, rhs);

	if (rhs_tmp == 0) {
		return NULL;
	}

	SkullInt lhs_tmp = 0;
	variable_read(&lhs_tmp, lhs);

	const SkullInt tmp = lhs_tmp / rhs_tmp;
	variable_write(ret, &tmp);

	return ret;
}

/*
Multiply `lhs` and `rhs` ints together
*/
Variable *mult_int_type(const Variable *lhs, const Variable *rhs) {
	DO_MATH(&TYPE_INT, SkullInt, *);
}
