#pragma once

#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char *fmt_float_type(const Variable *const);

Variable *add_float_type(const Variable *const, const Variable *const);
Variable *sub_float_type(const Variable *const, const Variable *const);
Variable *div_float_type(const Variable *const, const Variable *const);
Variable *mult_float_type(const Variable *const, const Variable *const);

void *eval_float(const Token *const, const char32_t **);
