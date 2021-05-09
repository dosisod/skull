#pragma once

#include <stdbool.h>

#include "skull/compiler/types/defs.h"
#include "skull/parse/token.h"

SkullStr eval_str(const Token *const, bool *);
