#pragma once

#include <stdbool.h>

#include "skull/compiler/types/defs.h"
#include "skull/parse/token.h"

SkullRune eval_rune(const Token *const, bool *);
