#pragma once

#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char32_t *fmt_bool_type(const Variable *);

void *eval_bool(const Token *, const char32_t **);
