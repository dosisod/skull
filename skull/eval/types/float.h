#pragma once

#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char *fmt_float_type(const Variable *const);

void *eval_float(const Token *const, const char32_t **);
