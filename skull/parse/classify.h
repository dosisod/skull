#pragma once

#include <uchar.h>

#include "skull/parse/token.h"

_Bool is_valid_identifier_str(const char32_t *const);

void classify_tokens(Token *);
