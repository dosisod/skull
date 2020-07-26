#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "skull/errors.h"
#include "skull/parse/classify.h"

#include "skull/eval/eval_float.h"

/*
Converts a `TOKEN_FLOAT_CONST` token to a floating point number (`long double`).

`error` is `NULL` if no error occurs, else `error` points to error msg.
*/
long double eval_float(const token_t *token, const char32_t **error) {
	if (token->token_type!=TOKEN_FLOAT_CONST) {
		*error=ERR_TYPE_MISMATCH;
		return 0.0L;
	}

	char *tmp=c32stombs(token->begin);

	if (strcmp("Infinity", tmp)==0) {
		return HUGE_VALL;
	}
	if (strcmp("-Infinity", tmp)==0) {
		return -HUGE_VALL;
	}

	const long double ret=strtold(tmp, NULL);

	free(tmp);
	return ret;
}
