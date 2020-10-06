#pragma once

#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char32_t *fmt_bool_type(const Variable *const);

void *eval_bool(const Token *const, const char32_t **);
