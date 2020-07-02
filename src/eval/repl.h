#pragma once

#include <stdio.h>

#include "../eval/context.h"
#include "../parse/tokenize.h"

#define REPL_MAX_LINE_LEN 1024

char32_t *repl_read(FILE *);

const char32_t *repl_make_var(const token_t *, context_t *, bool);
const char32_t *repl_eval(const char32_t *, context_t *);

void repl_loop(FILE *, context_t *);