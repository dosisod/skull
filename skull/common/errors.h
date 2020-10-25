#pragma once

#include <uchar.h>

#include "skull/parse/tokenize.h"

const char32_t *ERR_UNEXPECTED_TOKEN;
#define ERR_UNEXPECTED_TOKEN_(tok) U"unexpected token: \"" tok U"\""

const char32_t *ERR_EOF_NO_BRACKET;

const char32_t *ERR_TYPE_MISMATCH;

const char32_t *ERR_BAD_ESCAPE;
#define ERR_BAD_ESCAPE_(tok) U"bad string escape: \"" tok U"\""

const char32_t *ERR_VAR_NOT_FOUND;

const char32_t *ERR_OVERFLOW;
#define ERR_OVERFLOW_(str) U"overflow occurred while parsing \"" str U"\""

const char32_t *ERR_MISSING_ASSIGNMENT;

typedef struct Variable Variable;
typedef struct Type Type;

typedef struct {
	// dont store heap allocated strings in "str", do so in "real"
	const char32_t *str;
	const Token *tok;
	const Variable *var;
	const Type *type;

	// copy of string that will be used in final error msg
	char32_t *real;
} ErrorMsg;

char32_t *fmt_error(const char32_t *fmt, ErrorMsg []);
#define FMT_ERROR(fmt, ...) \
	fmt_error(fmt, (ErrorMsg[]){ \
		__VA_ARGS__, \
		{0} \
	})

void fmt_error_stringify(ErrorMsg *const);