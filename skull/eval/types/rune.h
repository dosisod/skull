#pragma once

#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char *fmt_rune_type(const Variable *const);

void *eval_rune(const Token *const, const char32_t **);
