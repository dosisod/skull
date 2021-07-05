#pragma once

typedef struct Location Location;
typedef struct Variable Variable;
typedef struct FunctionDeclaration FunctionDeclaration;
typedef const char * Type;

typedef enum {
	SYMBOL_UNKNOWN,
	SYMBOL_VAR,
	SYMBOL_FUNC,
	SYMBOL_ALIAS
} SymbolType;

typedef struct Symbol {
	char *name;
	Type expr_type;
	Location *location;
	SymbolType type;
	union {
		FunctionDeclaration *func;
		Variable *var;
	};
} Symbol;

_Bool is_valid_symbol(const Location *, char *, SymbolType);
_Bool scope_add_symbol(Symbol *);
