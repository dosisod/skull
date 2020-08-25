#pragma once

#include <stdbool.h>
#include <uchar.h>

#include "skull/parse/tokenize.h"

const char32_t *ERR_UNEXPECTED_TOKEN;
const char32_t *ERR_INVALID_INPUT;
const char32_t *ERR_TYPE_MISMATCH;

const char32_t *ERR_UNEXPECTED_PARAM;
#define _ERR_UNEXPECTED_PARAM(f) U"\"" f U"\" function does not take any parameters"

const char32_t *ERR_INVALID_PARAMS;
#define _ERR_INVALID_PARAMS(f) U"invalid number of parameters given in function \"" f U"\""

const char32_t *ERR_VAR_ALREADY_DEFINED;
#define _ERR_VAR_ALREADY_DEFINED(var) U"variable \"" var U"\" already defined"

const char32_t *ERR_VAR_NOT_FOUND;
#define _ERR_VAR_NOT_FOUND(var) U"variable \"" var U"\" not found"

const char32_t *ERR_CANNOT_ASSIGN_CONST;
#define _ERR_CANNOT_ASSIGN_CONST(var) U"cannot reassign const variable \"" var U"\""

const char32_t *ERR_OVERFLOW;
#define _ERR_OVERFLOW(str) U"overflow occurred while parsing \"" str U"\""

const char32_t *ERR_MISSING_ASSIGNMENT;
#define _ERR_MISSING_ASSIGNMENT(var) U"missing value in assignment to variable \"" var U"\""

const char32_t *ERR_NON_INT_RETURN;
#define _ERR_NON_INT_RETURN(var) U"returning non-int variable \"" var U"\" from main"

#define _ERR_CANNOT(do_action, type1, type2) \
	U"cannot " do_action U" type \"" type1 U"\" with type \"" type2 U"\""

#define _ERR_UNAVAILABLE(operation, type) \
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
#define _ERR_ASSIGN_FUNC(f) U"cannot reassign built-in function \"" f U"\""

struct variable_t;
struct type_t;

typedef struct {
	// dont store heap allocated strings in "str", do so in "real"
	const char32_t *str;
	const token_t *tok;
	const struct variable_t *var;
	const struct type_t *type;

	// copy of string that will be used in final error msg
	char32_t *real;
} error_msg_t;

char32_t *fmt_error(const char32_t *fmt, error_msg_t []);
#define FMT_ERROR(fmt, ...) \
	fmt_error(fmt, (error_msg_t[]){ \
		__VA_ARGS__, \
		{0} \
	})

void fmt_error_stringify(error_msg_t *);

bool is_error_msg(const char32_t *);