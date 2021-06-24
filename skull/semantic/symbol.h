#pragma once

#include "skull/parse/token.h"

typedef enum {
	SYMBOL_VAR,
	SYMBOL_FUNC,
	SYMBOL_ALIAS
} SymbolType;

_Bool is_valid_symbol(const Location *, char *, SymbolType);
