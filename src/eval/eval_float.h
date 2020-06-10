#pragma once

#include <errno.h>
#include <limits.h>

#include "../../src/errors.h"
#include "../../src/parse/classify.h"
#include "../../src/parse/tokenize.h"

/*
Converts a `TOKEN_FLOAT_CONST` token to a floating point number (`long double`).

Return `NULL` if no errors occurred, else, pointer to error msg.
*/
long double eval_float(const token_t *token, const wchar_t **error) {
	if (token->token_type!=TOKEN_FLOAT_CONST) {
		*error=ERROR_MSG[ERROR_TYPE_MISMATCH];
		return 0.0;
	}

	return wcstold(token->begin, NULL);
}
