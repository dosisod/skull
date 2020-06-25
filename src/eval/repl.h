#pragma once

#include <stdio.h>

#include "../eval/context.h"
#include "../parse/tokenize.h"

#define REPL_MAX_LINE_LEN 1024

/*
Returns pointer to string read from file descriptor `fd`.
*/
char32_t *repl_read(FILE *fd);

/*
Make and add a variable from passed `tokens` to context `ctx`.

Added variable will be constant if `is_const` is true.
*/
const char32_t *repl_make_var(const token_t *token, context_t *ctx, bool is_const);

/*
Evaluates a single line, returns result as a string (if any).
*/
const char32_t *repl_eval(const char32_t *str, context_t *ctx);