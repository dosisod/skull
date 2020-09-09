#pragma once

#include <uchar.h>

#include "skull/parse/tokenize.h"

const char32_t *ERR_UNEXPECTED_TOKEN;
#define ERR_UNEXPECTED_TOKEN_(tok) U"unexpected token: \"" tok U"\""

const char32_t *ERR_INVALID_INPUT;
#define ERR_INVALID_INPUT_(str) U"invalid input: \"" str U"\""

const char32_t *ERR_TYPE_MISMATCH;
#define ERR_TYPE_MISMATCH_(type) U"type mismatch: expected type \"" type U"\""

const char32_t *ERR_BAD_ESCAPE;
#define ERR_BAD_ESCAPE_(tok) U"bad string escape: \"" tok U"\""

const char32_t *ERR_UNEXPECTED_PARAM;
#define ERR_UNEXPECTED_PARAM_(f) U"\"" f U"\" function does not take any parameters"

const char32_t *ERR_INVALID_PARAMS;
#define ERR_INVALID_PARAMS_(f) U"invalid number of parameters given in function \"" f U"\""

const char32_t *ERR_VAR_ALREADY_DEFINED;
#define ERR_VAR_ALREADY_DEFINED_(var) U"variable \"" var U"\" already defined"

const char32_t *ERR_VAR_NOT_FOUND;
#define ERR_VAR_NOT_FOUND_(var) U"variable \"" var U"\" not found"

const char32_t *ERR_CANNOT_ASSIGN_CONST;
#define ERR_CANNOT_ASSIGN_CONST_(var) U"cannot reassign const variable \"" var U"\""

const char32_t *ERR_OVERFLOW;
#define ERR_OVERFLOW_(str) U"overflow occurred while parsing \"" str U"\""

const char32_t *ERR_MISSING_ASSIGNMENT;
#define ERR_MISSING_ASSIGNMENT_(var) U"missing value in assignment to variable \"" var U"\""

const char32_t *ERR_NON_INT_RETURN;
#define ERR_NON_INT_RETURN_(var) U"returning non-int variable \"" var U"\" from main"

#define ERR_CANNOT_(do_action, type1, type2) \
	U"cannot " do_action U" type \"" type1 U"\" with type \"" type2 U"\""

#define ERR_UNAVAILABLE_(operation, type) \
	U"type \"" type U"\" does not allow for " operation

const char32_t *ERR_CANNOT_ADD;
const char32_t *ERR_ADD_UNAVAILABLE;

const char32_t *ERR_CANNOT_SUB;
const char32_t *ERR_SUB_UNAVAILABLE;

const char32_t *ERR_CANNOT_MULT;
const char32_t *ERR_MULT_UNAVAILABLE;

const char32_t *ERR_CANNOT_DIV;
const char32_t *ERR_DIV_UNAVAILABLE;

const char32_t *ERR_TYPE_TYPE_BAD;

const char32_t *ERR_ASSIGN_FUNC;
#define ERR_ASSIGN_FUNC_(f) U"cannot reassign built-in function \"" f U"\""

struct Variable;

typedef struct {
	// dont store heap allocated strings in "str", do so in "real"
	const char32_t *str;
	const Token *tok;
	const struct Variable *var;
	const struct Type *type;

	// copy of string that will be used in final error msg
	char32_t *real;
} ErrorMsg;

char32_t *fmt_error(const char32_t *fmt, ErrorMsg []);
#define FMT_ERROR(fmt, ...) \
	fmt_error(fmt, (ErrorMsg[]){ \
		__VA_ARGS__, \
		{0} \
	})

void fmt_error_stringify(ErrorMsg *);

_Bool is_error_msg(const char32_t *);