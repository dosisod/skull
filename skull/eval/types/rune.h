#pragma once

#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char32_t *fmt_rune_type(const Variable *);

void *eval_rune(const Token *, const char32_t **);
