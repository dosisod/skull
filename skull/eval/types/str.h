#pragma once

#include "skull/eval/types/defs.h"
#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char *fmt_str_type(const Variable *const);

SkullStr eval_str(const Token *const, char32_t **);
