#pragma once

#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char32_t *fmt_int_type(const Variable *);

Variable *add_int_type(const Variable *, const Variable *);
Variable *sub_int_type(const Variable *, const Variable *);
Variable *div_int_type(const Variable *, const Variable *);
Variable *mult_int_type(const Variable *, const Variable *);

void *eval_integer(const Token *, const char32_t **);
