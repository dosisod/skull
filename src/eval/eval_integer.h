#pragma once

#include "../../src/parse/tokenize.h"

/*
Converts a `TOKEN_INT_CONST` token to an actual integer (`int64_t`).

`error` is `NULL` if no error occurs, else `error` points to error msg.
*/
int64_t eval_integer(const token_t *token, const char32_t **error);
