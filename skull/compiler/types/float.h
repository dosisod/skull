#pragma once

#include <stdbool.h>

#include "skull/compiler/types/defs.h"
#include "skull/parse/token.h"

SkullFloat eval_float(const Token *const, bool *);
