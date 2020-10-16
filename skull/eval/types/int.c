#include <errno.h>
#include <limits.h>

#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/eval/types/sprint_fmt.h"
#include "skull/parse/classify.h"

#include "skull/eval/types/int.h"

/*
Returns the string representation of int `var`
*/
char *fmt_int_type(const Variable *const var) {
	SkullInt data = 0;
	variable_read(&data, var);

	SPRINTF_FMT("%li");
	return tmp;
}

/*
Returns an Skull integer parsed from `token`.

`error` is `NULL` if no error occurs, else `error` points to error msg.
*/
SkullInt eval_integer(const Token *const token, char32_t **error) {
	const char32_t *begin = token->begin;
	int base = 10;

	if (c32sncmp(U"0b", token->begin, 2)) {
		begin += 2;
		base = 2;
	}
	else if (c32sncmp(U"0o", token->begin, 2)) {
		begin += 2;
		base = 8;
	}
	else if (c32sncmp(U"0x", token->begin, 2)) {
		begin += 2;
		base = 16;
	}

	char *const tmp = c32stombs(begin);

	errno = 0;
	SkullInt ret = strtoll(tmp, NULL, base);
	free(tmp);

	if ((ret == LLONG_MAX || ret == LLONG_MIN) && errno == ERANGE) {
		*error = FMT_ERROR(ERR_OVERFLOW, { .tok = token });
	}

	return ret;
}
