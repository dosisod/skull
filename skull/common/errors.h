#pragma once

#include <uchar.h>

#include "skull/parse/token.h"

#define ERR_UNEXPECTED_TOKEN "unexpected token: \"%s\"\n"
#define ERR_EOF_NO_BRACKET "Reached EOF, expected closing bracket"
#define ERR_TYPE_MISMATCH "type mismatch: expected type \"%s\"\n"
#define ERR_FUNC_TYPE_MISMATCH "expected param of type \"%s\", got \"%s\"\n"
#define ERR_EXPECTED_SAME_TYPE "expected type \"%s\", got \"%s\"\n"
#define ERR_ASSIGN_BAD_TYPE "cannot assign type \"%s\" to type \"%s\"\n"
#define ERR_BAD_ESCAPE "bad string escape: \"%s\"\n"
#define ERR_VAR_NOT_FOUND "variable \"%s\" not found\n"
#define ERR_OVERFLOW "overflow occurred while parsing \"%s\"\n"
#define ERR_FILE_EMPTY "file is empty"
#define ERR_MISSING_OPEN_BRAK "missing opening bracket"
#define ERR_MISSING_CLOSING_PAREN "missing closing parenthesis"
#define ERR_MISSING_DECLARATION "function \"%s\" missing declaration\n"
#define ERR_MISSING_BLOCK "%s statement must be followed by code block\n"
#define ERR_NO_CLOSING_COMMENT "expected closing \"#}\" for block comment"
#define ERR_NO_CLOSING_QUOTE "expected closing quote"
#define ERR_NO_VOID_ASSIGN \
	"function returning type void cannot be assigned to variable \"%s\"\n"
#define ERR_INVALID_INPUT "invalid input: \"%s\"\n"
#define ERR_VAR_ALREADY_DEFINED "variable \"%s\" already defined\n"
#define ERR_NON_INT_MAIN "returning non-int expression \"%s\" from main\n"
#define ERR_NON_BOOL_EXPR "expected boolean expression"
#define ERR_MAIN_RESERVED "cannot export function \"main\""
#define ERR_NO_REDEFINE_FUNC "cannot redeclare function \"%s\"\n"
#define ERR_ZERO_PARAM_FUNC \
	"passing parameter to function that takes zero parameters"
#define ERR_SHADOW_VAR "variable \"%s\" shadows existing variable\n"
#define ERR_EXPECTED_RETURN "expected return value in function \"%s\"\n"
#define ERR_NO_VOID_RETURN "unexpected return from void function \"%s\"\n"
#define ERR_ELSE_MISSING_IF "else statement missing preceding if statement"
#define ERR_UNEXPECTED_CODE_BLOCK "unexpected code block"
#define ERR_EMPTY_BLOCK "code block cannot be empty"
#define ERR_UNREACHABLE_CODE "unreachable code after return"
#define ERR_REASSIGN_CONST "cannot reassign const variable \"%s\"\n"
#define ERR_INVALID_EXPR "invalid expression near \"%s\"\n"
#define ERR_NO_DANGLING_EXPR "expression cannot be used on its own"
#define ERR_REDUNDANT_REASSIGN \
	"redundant assignment of variable \"%s\" to itself\n"
#define ERR_DIV_BY_ZERO "division by zero"
#define ERR_NO_NESTED \
	"cannot declare nested function \"%s\" as external or exported\n"
#define ERR_NOT_COMPARIBLE "cannot compare value near \"%s\"\n"
#define ERR_EXPECTED_COMMA "expected comma"
#define ERR_UNCLOSED_FUNC_CALL "function call missing closing parenthesis"
#define ERR_TYPE_NOT_FOUND "type \"%s\" could not be found\n"
#define ERR_ALIAS_ALREADY_DEFINED "alias \"%s\" is already defined\n"
#define ERR_ASSIGN_MISSING_EXPR "expected expression in assignment variable"
#define ERR_RETURN_MISSING_EXPR "expected expression in return"
#define ERR_EXPECTED_EXPR "expected expression after \"%s\"\n"
#define ERR_NO_CONTROL_CHAR "control character cannot be used in rune"
#define ERR_INVALID_NUM_PARAMS "invalid number of parameters"
#define ERR_NESTED_BLOCK_COMMENT \
	"cannot put opening block comment in existing block comment"
#define ERR_INVALID_COMMENT_START "invalid start of comment"
#define ERR_ILLEGAL_SEQ_AT "illegal UTF8 sequence at character offset %s\n"
#define ERR_ILLEGAL_SEQ "illegal UTF8 sequence in this region"
#define ERR_POW_BAD_TYPE "cannot use type \"%s\" for power operator\n"
#define WARN_VAR_NOT_CONST "variable \"%s\" should be constant\n"
#define WARN_VAR_UNUSED "variable \"%s\" is unused\n"
#define WARN_COND_ALWAYS_TRUE "condition is always true"
#define WARN_COND_ALWAYS_FALSE "condition is always false"

typedef struct Variable Variable;
typedef const char * Type;

/*
`ErrorMsg` stores a reference to a given object, generating an error msg based
on which one is set.

See `fmt_error_stringify` for uses of these fields.
*/
typedef struct {
	const char32_t *str;
	const Token *tok;
	const Location *loc;
	const Variable *var;
	Type type;
	size_t i;

	// copy of string that will be used in final error msg
	char *real;
} ErrorMsg;

typedef enum {
	ERROR_FATAL,
	ERROR_WARN
} ErrorType;

#define FMT_WARN(fmt, ...) \
	char *_fmt_error_str = fmt_error(ERROR_WARN, fmt, (ErrorMsg[]){ \
		__VA_ARGS__, \
		{0} \
	}); \
	fprintf(stderr, "%s", _fmt_error_str); \
	free(_fmt_error_str)

#define FMT_ERROR(fmt, ...) \
	char *_fmt_error_str = fmt_error(ERROR_FATAL, fmt, (ErrorMsg[]){ \
		__VA_ARGS__, \
		{0} \
	}); \
	fprintf(stderr, "%s", _fmt_error_str); \
	free(_fmt_error_str)

char *fmt_error(ErrorType, const char *fmt, ErrorMsg []);

void fmt_error_stringify(ErrorMsg *const);
