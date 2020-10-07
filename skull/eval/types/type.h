#pragma once

#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char *fmt_type_type(const Variable *const);

void *eval_type(const Token *const, const char32_t **);
