#pragma once

#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char *fmt_int_type(const Variable *);

void *eval_integer(const Token *, const char32_t **);
