#include <math.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/eval/eval_oper.h"
#include "skull/eval/types/sprint_fmt.h"

#include "skull/eval/types/float.h"

/*
Returns the string representation of float `var`
*/
char32_t *fmt_float_type(const variable_t *var) {
	double data = 0.0;
	variable_read(&data, var);

	if (isinf(data)) {
		char32_t *ret = NULL;
		if (data < 0.0) {
			ret = c32sdup(U"-Infinity");
		}
		else {
			ret = c32sdup(U"Infinity");
		}
		DIE_IF_MALLOC_FAILS(ret);
		return ret;
	}

	SPRINTF_FMT("%.16lg");

	const char *period = strrchr(tmp, '.');
	if (!period && !strrchr(tmp, 'e')) {
		const size_t len = strlen(tmp);
		char *fixed = malloc(len + 3);
		DIE_IF_MALLOC_FAILS(fixed);

		memcpy(fixed, tmp, len);
		fixed[len] = '.';
		fixed[len + 1] = '0';
		fixed[len + 2] = '\0';

		free(tmp);
		tmp = fixed;
	}

	char32_t *ret = mbstoc32s(tmp);
	free(tmp);
	return ret;
}

/*
Add `lhs` and `rhs` floats together
*/
variable_t *add_float_type(const variable_t *lhs, const variable_t *rhs) {
	DO_MATH(U"float", double, +);
}

/*
Subtract `rhs` float from `lhs` float
*/
variable_t *sub_float_type(const variable_t *lhs, const variable_t *rhs) {
	DO_MATH(U"float", double, -);
}

/*
Divide `lhs` float by `rhs` float
*/
variable_t *div_float_type(const variable_t *lhs, const variable_t *rhs) {
	DO_MATH(U"float", double, /);
}

/*
Multiply `lhs` and `rhs` floats together
*/
variable_t *mult_float_type(const variable_t *lhs, const variable_t *rhs) {
	DO_MATH(U"float", double, *);
}
