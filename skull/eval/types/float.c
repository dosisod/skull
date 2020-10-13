#include <errno.h>
#include <math.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/eval/types/defs.h"
#include "skull/eval/types/sprint_fmt.h"
#include "skull/parse/classify.h"

#include "skull/eval/types/float.h"

/*
Returns the string representation of float `var`
*/
char *fmt_float_type(const Variable *const var) {
	SkullFloat data = 0.0;
	variable_read(&data, var);

	if (isinf(data)) {
		char *const ret = strdup(
			(data < 0.0) ?
			"-Infinity" :
			"Infinity"
		);

		DIE_IF_MALLOC_FAILS(ret);
		return ret;
	}

	SPRINTF_FMT("%.16lg");

	const char *const period = strrchr(tmp, '.');
	if (!period && !strrchr(tmp, 'e')) {
		const size_t len = strlen(tmp);
		char *const fixed = malloc(len + 3);
		DIE_IF_MALLOC_FAILS(fixed);

		memcpy(fixed, tmp, len);
		fixed[len] = '.';
		fixed[len + 1] = '0';
		fixed[len + 2] = '\0';

		free(tmp);
		tmp = fixed;
	}

	return tmp;
}

/*
Converts a `TOKEN_FLOAT_CONST` token to a floating point number pointer (`SkullFloat *`).

`error` is `NULL` if no error occurs, else `error` points to error msg.
*/
void *eval_float(const Token *const token, char32_t **error) {
	char *const tmp = c32stombs(token->begin);

	SkullFloat *ret;
	ret = malloc(sizeof *ret);
	DIE_IF_MALLOC_FAILS(ret);

	if (strcmp("Infinity", tmp) == 0) {
		*ret = HUGE_VAL;
		return ret;
	}
	if (strcmp("-Infinity", tmp) == 0) {
		*ret = -HUGE_VAL;
		return ret;
	}

	errno = 0;
	*ret = strtod(tmp, NULL);
	free(tmp);

	if (isinf(*ret) && errno == ERANGE) {
		*error = FMT_ERROR(ERR_OVERFLOW, { .tok = token });
	}

	return ret;
}
