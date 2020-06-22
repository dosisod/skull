#pragma once

#include <errno.h>
#include <limits.h>

#include "../../src/errors.h"
#include "../../src/parse/classify.h"
#include "../../src/parse/tokenize.h"

/*
Converts a `TOKEN_FLOAT_CONST` token to a floating point number (`long double`).

`error` is `NULL` if no error occurs, else `error` points to error msg.
*/
long double eval_float(const token_t *token, const char32_t **error) {
	if (token->token_type!=TOKEN_FLOAT_CONST) {
		*error=ERROR_MSG[ERROR_TYPE_MISMATCH];
		return 0.0;
	}

	char *tmp=c32stombs(token->begin);
	const long double ret=strtold(tmp, NULL);

	free(tmp);
	return ret;
}
