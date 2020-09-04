#pragma once

#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char32_t *fmt_type_type(const Variable *);

void *eval_type(const Token *, const char32_t **);
