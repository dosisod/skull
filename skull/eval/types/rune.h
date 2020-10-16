#pragma once

#include "skull/eval/types/defs.h"
#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char *fmt_rune_type(const Variable *const);

SkullRune eval_rune(const Token *const, char32_t **);
