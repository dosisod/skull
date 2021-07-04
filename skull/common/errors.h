#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <uchar.h>

typedef struct Token Token;
typedef struct Location Location;

typedef enum {
	ERR_UNEXPECTED_TOKEN = 0,
	ERR_EOF_NO_BRACKET = 1,
	ERR_TYPE_MISMATCH = 2, // unsued
	ERR_FUNC_TYPE_MISMATCH = 3,
	ERR_EXPECTED_SAME_TYPE = 4,
	ERR_ASSIGN_BAD_TYPE = 5,
	ERR_BAD_ESCAPE = 6,
	ERR_VAR_NOT_FOUND = 7,
	ERR_OVERFLOW = 8,
	ERR_MISSING_OPEN_BRAK = 9,
	ERR_MISSING_CLOSING_PAREN = 10,
	ERR_MISSING_DECLARATION = 11,
	ERR_MISSING_BLOCK = 12,
	ERR_NO_CLOSING_COMMENT = 13,
	ERR_NO_CLOSING_QUOTE = 14,
	ERR_NO_VOID_ASSIGN = 15,
	ERR_INVALID_INPUT = 16,
	ERR_VAR_ALREADY_DEFINED = 17,
	ERR_NON_INT_MAIN = 18,
	ERR_NON_BOOL_EXPR = 19,
	ERR_MAIN_RESERVED = 20,
	ERR_NO_REDEFINE_FUNC = 21,
	ERR_NO_REDEFINE_VAR_AS_FUNC = 22,
	ERR_NO_REDEFINE_FUNC_AS_VAR = 23,
	ERR_ZERO_PARAM_FUNC = 24,
	ERR_SHADOW_VAR = 25,
	ERR_EXPECTED_RETURN = 26,
	ERR_NO_VOID_RETURN = 27,
	ERR_ELSE_ELIF_MISSING_IF = 28,
	ERR_UNEXPECTED_CODE_BLOCK = 29,
	ERR_EMPTY_BLOCK = 30,
	ERR_UNREACHABLE_CODE = 31,
	ERR_REASSIGN_CONST = 32,
	ERR_INVALID_EXPR = 33,
	ERR_NO_DANGLING_EXPR = 34,
	ERR_REDUNDANT_REASSIGN = 35,
	ERR_DIV_BY_ZERO = 36,
	ERR_NO_NESTED = 37,
	ERR_TOP_LVL_ALIAS_ONLY = 38,
	ERR_NOT_COMPARIBLE = 39,
	ERR_EXPECTED_COMMA = 40,
	ERR_UNCLOSED_FUNC_CALL = 41,
	ERR_TYPE_NOT_FOUND = 42,
	ERR_ALIAS_ALREADY_DEFINED = 43,
	ERR_ASSIGN_MISSING_EXPR = 44,
	ERR_RETURN_MISSING_EXPR = 45,
	ERR_EXPECTED_EXPR = 46,
	ERR_NO_CONTROL_CHAR = 47,
	ERR_INVALID_NUM_PARAMS = 48,
	ERR_INVALID_COMMENT_START = 49,
	ERR_ILLEGAL_SEQ_AT = 50,
	ERR_ILLEGAL_SEQ = 51,
	ERR_POW_BAD_TYPE = 52,
	ERR_EXPECTED_NEWLINE = 53,
	WARN_VAR_NOT_CONST = 54,
	WARN_VAR_UNUSED = 55,
	WARN_FUNC_UNUSED = 56,
	WARN_COND_ALWAYS_TRUE = 57,
	WARN_COND_ALWAYS_FALSE = 58,
	WARN_FILE_EMPTY = 59,
	WARN_NESTED_BLOCK_COMMENT = 60,
	WARN_TRIVIAL_TYPE = 61,
	ERR_NO_REDEFINE_ALIAS_AS_VAR = 62,
	ERR_NO_REDEFINE_ALIAS_AS_FUNC = 63,
	ERR_NO_REDEFINE_VAR_AS_ALIAS = 64,
	ERR_NO_REDEFINE_FUNC_AS_ALIAS = 65
} ErrorCode;

typedef struct Variable Variable;
typedef const char * Type;

/*
`ErrorMsg` stores a reference to a given object, generating an error msg based
on which one is set.

See `fmt_error_stringify` for uses of these fields.
*/
typedef struct {
	const char *str;
	const char32_t *str32;
	const Token *tok;
	const Location *loc;
	const Variable *var;
	Type type;
	char *real; // copy of string that will be used in final error msg

	size_t i;
} ErrorMsg;

typedef enum {
	ERROR_FATAL,
	ERROR_WARN
} ErrorType;

#define FMT_WARN(fmt, ...) { \
	char *_fmt_error_str = fmt_error(ERROR_WARN, fmt, (ErrorMsg[]){ \
		__VA_ARGS__, \
		{0} \
	}); \
	fprintf(stderr, "%s", _fmt_error_str); \
	free(_fmt_error_str); \
}

#define FMT_ERROR(fmt, ...) { \
	char *_fmt_error_str = fmt_error(ERROR_FATAL, fmt, (ErrorMsg[]){ \
		__VA_ARGS__, \
		{0} \
	}); \
	fprintf(stderr, "%s", _fmt_error_str); \
	free(_fmt_error_str); \
}

char *fmt_error(ErrorType, ErrorCode, ErrorMsg []);
