#pragma once

#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char32_t *fmt_char_type(const variable_t *);

void *eval_char(const token_t *, const char32_t **);
