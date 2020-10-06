#pragma once

#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char32_t *fmt_int_type(const Variable *);

Variable *add_int_type(const Variable *const, const Variable *const);
Variable *sub_int_type(const Variable *const, const Variable *const);
Variable *div_int_type(const Variable *const, const Variable *const);
Variable *mult_int_type(const Variable *const, const Variable *const);

void *eval_integer(const Token *, const char32_t **);
