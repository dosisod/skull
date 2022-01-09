#pragma once

#include "skull/semantic/types.h"

typedef struct Location Location;
typedef struct Variable Variable;
typedef struct FunctionDeclaration FunctionDeclaration;

typedef enum {
	SYMBOL_UNKNOWN,
	SYMBOL_VAR,
	SYMBOL_FUNC,
	SYMBOL_ALIAS
} SymbolType;

typedef struct Symbol {
	char *name;
	Type *expr_type;
	const Location *location;
	SymbolType type;
	union {
		FunctionDeclaration *func;
		Variable *var;
	};
} Symbol;

_Bool scope_add_symbol(Symbol *);
