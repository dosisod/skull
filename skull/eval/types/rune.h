#pragma once

#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char32_t *fmt_rune_type(const variable_t *);

void *eval_rune(const token_t *, const char32_t **);
