#pragma once

#include "skull/semantic/shared.h"
#include "skull/semantic/types.h"

typedef struct Location Location;
typedef struct Variable Variable;
typedef struct FunctionDeclaration FunctionDeclaration;

typedef enum {
	SYMBOL_UNKNOWN,
	SYMBOL_VAR,
	SYMBOL_FUNC,
	SYMBOL_ALIAS,
	SYMBOL_NAMESPACE,
} SymbolType;

typedef struct Symbol {
	char *name;
	char *linkage_name;
	const Type *expr_type;
	Location location;
	SymbolType type;
	union {
		FunctionDeclaration *func;
		Variable *var;
		Scope *scope;
	};
} Symbol;

_Bool scope_add_symbol(SemanticState *, Symbol *);
