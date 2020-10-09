#pragma once

#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char *fmt_str_type(const Variable *const);

void *eval_str(const Token *const, const char32_t **);
