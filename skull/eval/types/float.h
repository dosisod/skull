#include <math.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/eval/eval_oper.h"
#include "skull/eval/types/sprint_fmt.h"
#include "skull/eval/variable.h"

char32_t *fmt_float_type(const variable_t *var) {
	long double data=0;
	variable_read(&data, var);

	if (isinf(data)) {
		char32_t *ret=NULL;
		if (data < 0.0L) {
			ret=c32sdup(U"-Infinity");
		}
		else {
			ret=c32sdup(U"Infinity");
		}
		DIE_IF_MALLOC_FAILS(ret);
		return ret;
	}

	SPRINTF_FMT("%Lf");

	//trim excess zeros off of decimal
	for (size_t i=strlen(tmp); i>0; i--) {
		if (tmp[i-1]=='.') {
			tmp[i]='0';
		}
		else if (tmp[i-1]=='0') {
			tmp[i-1]='\0';
			continue;
		}
		break;
	}

	char32_t *ret=mbstoc32s(tmp);
	free(tmp);
	return ret;
}

variable_t *add_float_type(const variable_t *lhs, const variable_t *rhs) {
	DO_MATH(U"float", long double, +);
}

variable_t *sub_float_type(const variable_t *lhs, const variable_t *rhs) {
	DO_MATH(U"float", long double, -);
}

variable_t *div_float_type(const variable_t *lhs, const variable_t *rhs) {
	DO_MATH(U"float", long double, /);
}

variable_t *mult_float_type(const variable_t *lhs, const variable_t *rhs) {
	DO_MATH(U"float", long double, *);
}