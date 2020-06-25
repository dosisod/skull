#pragma once

#include "context.h"
#include "../parse/tokenize.h"
#include "variable.h"

/*
Assign the value of `token` to a variable `var`.

Set `ctx` to allow for assigning variables to other variables.

Return an error (as a string) if any occured, else `NULL`.
*/
const char32_t *eval_assign(variable_t *var, token_t *token, const context_t *ctx);