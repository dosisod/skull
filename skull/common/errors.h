#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <uchar.h>

#include "skull/semantic/types.h"

typedef struct Token Token;
typedef struct Location Location;
typedef struct Vector Vector;

// unused error codes: 20, 22, 23, 38, 39, 62, 63, 64, 65

typedef enum {
	ERR_UNEXPECTED_TOKEN = 0,
	ERR_EOF_NO_BRACKET = 1,
	ERR_NOT_INT = 2,
	ERR_FUNC_TYPE_MISMATCH = 3,
	ERR_EXPECTED_SAME_TYPE = 4,
	ERR_RUNE_TOO_LONG = 5,
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
	ERR_IMPORT_IDENT = 16,
	ERR_VAR_ALREADY_DEFINED = 17,
	ERR_NON_INT_MAIN = 18,
	ERR_NON_BOOL_EXPR = 19,
	ERR_NO_REDEFINE_FUNC = 21,
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
	ERR_NESTED_BLOCK_COMMENT = 60,
	WARN_TRIVIAL_TYPE = 61,
	ERR_NOT_NUMERIC = 66,
	WARN_NO_BOM = 67,
	ERR_BREAK_NOT_IN_WHILE = 68,
	ERR_CONTINUE_NOT_IN_WHILE = 69,
	ERR_NO_MUT_TYPE_ALIAS = 70,
	ERR_NO_TYPE_EXPR = 71,
	ERR_NO_DOUBLE_UNARY = 72,
	ERR_UNEXPECTED_UNARY_EXPR = 73,
	ERR_REF_IDENT_ONLY = 74,
	ERR_MISSING_ASSIGNMENT = 75,
	ERR_UNEXPECTED_COMMA = 76,
	ERR_CANNOT_DEREF = 77,
	ERR_LOAD_MODULE = 78,
	ERR_NO_REDEFINE_X_AS_Y = 79,
} ErrorCode;

typedef struct Variable Variable;

/*
`Message` allows for passing different objects (strings, types, variables),
and have the proper string representations generated automatically.

See `message_stringify` for uses of these fields.
*/
typedef struct {
	const char *str;
	const char32_t *str32;
	const Token *tok;
	const Location *loc;
	const Variable *var;
	const Type *type;
	char *real; // copy of string that will be used in final error msg

	size_t i;
} Message;

typedef enum {
	ERROR_FATAL,
	ERROR_WARN
} ErrorType;

extern Vector *error_msgs;

#define FMT_WARN(err, id, ...) { \
	fmt_message(ERROR_WARN, (id), (Message[]){ __VA_ARGS__, {0} }); \
	if (BUILD_DATA.werror) (err) = true; \
}

#define FMT_ERROR(id, ...) \
	fmt_message(ERROR_FATAL, (id), (Message[]){ __VA_ARGS__, {0} })

void fmt_message(ErrorType, ErrorCode, Message []);

void write_and_free_errors(void);
void free_errors(void);
