#pragma once

#include "skull/eval/types/defs.h"
#include "skull/parse/tokenize.h"

SkullStr eval_str(const Token *const, char32_t **);
