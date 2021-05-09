#pragma once

#include <stdbool.h>

#include "skull/compiler/types/defs.h"
#include "skull/parse/token.h"

SkullInt eval_integer(const Token *, bool *);
