#pragma once

#include <stdbool.h>

#include "skull/parse/token.h"

char32_t eval_rune(const Token *const, bool *);
