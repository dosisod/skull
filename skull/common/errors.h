#pragma once

#include <uchar.h>

#include "skull/parse/tokenize.h"

#define ERR_UNEXPECTED_TOKEN ("unexpected token: \"%s\"\n")
#define ERR_EOF_NO_BRACKET ("Reached EOF, expected closing bracket\n")
#define ERR_TYPE_MISMATCH ("type mismatch: expected type \"%s\"")
#define ERR_BAD_ESCAPE ("bad string escape: \"%s\"\n")
#define ERR_VAR_NOT_FOUND ("variable \"%s\" not found\n")
#define ERR_OVERFLOW ("overflow occurred while parsing \"%s\"\n")
#define ERR_MISSING_ASSIGNMENT ("missing value in assignment to variable \"%s\"\n")
#define ERR_TYPE_MISMATCH ("type mismatch: expected type \"%s\"")

typedef struct Variable Variable;
typedef struct Type Type;

typedef struct {
	// dont store heap allocated strings in "str", do so in "real"
	const char32_t *str;
	const Token *tok;
	const Variable *var;
	const Type *type;

	// copy of string that will be used in final error msg
	char *real;
} ErrorMsg;

void fmt_error(const char *fmt, ErrorMsg []);

#define FMT_ERROR(fmt, ...) \
	fmt_error(fmt, (ErrorMsg[]){ \
		__VA_ARGS__, \
		{0} \
	})

void fmt_error_stringify(ErrorMsg *const);