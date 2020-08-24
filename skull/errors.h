#pragma once

#include <stdbool.h>
#include <uchar.h>

#include "skull/parse/tokenize.h"

const char32_t *ERR_UNEXPECTED_TOKEN;
const char32_t *ERR_INVALID_INPUT;
const char32_t *ERR_VAR_ALREADY_DEFINED;
const char32_t *ERR_CANNOT_ASSIGN_CONST;
const char32_t *ERR_TYPE_MISMATCH;
const char32_t *ERR_OVERFLOW;
const char32_t *ERR_MISSING_ASSIGNMENT;
const char32_t *ERR_VAR_NOT_FOUND;
const char32_t *ERR_NON_INT_RETURN;
const char32_t *ERR_CANNOT_ADD;
const char32_t *ERR_ADD_UNAVAILABLE;
const char32_t *ERR_CANNOT_SUB;
const char32_t *ERR_SUB_UNAVAILABLE;
const char32_t *ERR_CANNOT_MULT;
const char32_t *ERR_MULT_UNAVAILABLE;
const char32_t *ERR_CANNOT_DIV;
const char32_t *ERR_DIV_UNAVAILABLE;
const char32_t *ERR_TYPE_TYPE_BAD;
const char32_t *ERR_MISSING_TOKEN;
const char32_t *ERR_UNEXPECTED_PARAM;
const char32_t *ERR_INVALID_PARAMS;
const char32_t *ERR_ASSIGN_FUNC;

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

void fmt_error_stringify(error_msg_t *);

bool is_error_msg(const char32_t *);