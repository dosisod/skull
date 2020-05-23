#pragma once

#include <errno.h>
#include <limits.h>

#include "../../src/parse/classify.h"
#include "../../src/parse/tokenize.h"

#define EVAL_FLOAT_OK 0
#define EVAL_FLOAT_ERR 1

/*
Converts a `TOKEN_FLOAT_CONST` token to a floating point number (`long double`).

If an error occurs while converting, `error` is set to `EVAL_FLOAT_ERR`.
`error` should always be `EVAL_FLOAT_OK` upon success.
*/
long double eval_float(const token_t *token, uint8_t *error) {
	*error=EVAL_FLOAT_OK;

	if (token->token_type!=TOKEN_FLOAT_CONST) {
		*error=EVAL_FLOAT_ERR;
		return 0.0;
	}

	errno=0;
	long double ret=wcstold(token->begin, NULL);

	if (errno==ERANGE) {
		*error=EVAL_FLOAT_ERR;
	}

	return ret;
}
