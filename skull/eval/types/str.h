#pragma once

#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char32_t *fmt_str_type(const variable_t *);

variable_t *add_str_type(const variable_t *, const variable_t *);

void *eval_str(const token_t *, const char32_t **);
