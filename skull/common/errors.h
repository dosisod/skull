#pragma once

#include <uchar.h>

#include "skull/parse/tokenize.h"

#define ERR_UNEXPECTED_TOKEN ("unexpected token: \"%s\"\n")
#define ERR_EOF_NO_BRACKET ("Reached EOF, expected closing bracket")
#define ERR_TYPE_MISMATCH ("type mismatch: expected type \"%s\"\n")
#define ERR_FUNC_TYPE_MISMATCH ("type mismatch: expected param of type \"%s\"\n")
#define ERR_BAD_ESCAPE ("bad string escape: \"%s\"\n")
#define ERR_VAR_NOT_FOUND ("variable \"%s\" not found\n")
#define ERR_OVERFLOW ("overflow occurred while parsing \"%s\"\n")
#define ERR_MISSING_ASSIGNMENT ("missing value in assignment to variable \"%s\"\n")
#define ERR_FILE_EMPTY ("file is empty")
#define ERR_MISSING_OPEN_BRAK ("missing opening bracket")
#define ERR_MISSING_EXTERNAL ("external function \"%s\" missing external declaration\n")
#define ERR_MISSING_BLOCK ("%s statement must be followed by code block\n")

typedef struct Variable Variable;
typedef struct Type Type;

/*
`ErrorMsg` stores a reference to a given object, generating an error msg based on which one is set.

See `fmt_error_stringify` for uses of these fields.
*/
typedef struct {
	const char32_t *str;
	const Token *tok;
	const Variable *var;
	const Type *type;

	// copy of string that will be used in final error msg
	char *real;
} ErrorMsg;

void fmt_error(const char *fmt, ErrorMsg []);

void fmt_error_stringify(ErrorMsg *const);