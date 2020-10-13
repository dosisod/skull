#include <errno.h>
#include <limits.h>

#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/eval/types/defs.h"
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
Converts a `TOKEN_INT_CONST` token to an integer pointer (`SkullInt *`).

`error` is `NULL` if no error occurs, else `error` points to error msg.
*/
void *eval_integer(const Token *const token, char32_t **error) {
	SkullInt *ret;
	ret = malloc(sizeof *ret);
	DIE_IF_MALLOC_FAILS(ret);

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
	*ret = strtoll(tmp, NULL, base);
	free(tmp);

	if ((*ret == LLONG_MAX || *ret == LLONG_MIN) && errno == ERANGE) {
		*error = FMT_ERROR(ERR_OVERFLOW, { .tok = token });
	}

	return ret;
}
