#pragma once

#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char32_t *fmt_str_type(const Variable *);

Variable *add_str_type(const Variable *, const Variable *);

void *eval_str(const Token *, const char32_t **);
