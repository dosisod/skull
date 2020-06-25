#pragma once

#include "../../src/parse/tokenize.h"

/*
Converts a `TOKEN_FLOAT_CONST` token to a floating point number (`long double`).

`error` is `NULL` if no error occurs, else `error` points to error msg.
*/
long double eval_float(const token_t *token, const char32_t **error);
