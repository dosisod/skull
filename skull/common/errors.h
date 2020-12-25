#pragma once

#include <uchar.h>

#include "skull/parse/token.h"

#define ERR_UNEXPECTED_TOKEN "unexpected token: \"%s\"\n"
#define ERR_EOF_NO_BRACKET "Reached EOF, expected closing bracket"
#define ERR_TYPE_MISMATCH "type mismatch: expected type \"%s\"\n"
#define ERR_FUNC_TYPE_MISMATCH "type mismatch: expected param of type \"%s\"\n"
#define ERR_BAD_ESCAPE "bad string escape: \"%s\"\n"
#define ERR_VAR_NOT_FOUND "variable \"%s\" not found\n"
#define ERR_OVERFLOW "overflow occurred while parsing \"%s\"\n"
#define ERR_MISSING_ASSIGNMENT "missing value in assignment to variable \"%s\"\n"
#define ERR_FILE_EMPTY "file is empty"
#define ERR_MISSING_OPEN_BRAK "missing opening bracket\n"
#define ERR_MISSING_DECLARATION "function \"%s\" missing declaration\n"
#define ERR_MISSING_BLOCK "%s statement must be followed by code block\n"
#define ERR_NO_CLOSING_COMMENT "expected closing \"#}\" for block comment\n"
#define ERR_NO_VOID_ASSIGN "function returning type void cannot be assigned to variable \"%s\"\n"
#define ERR_INVALID_INPUT "invalid input: \"%s\"\n"
#define ERR_VAR_ALREADY_DEFINED "variable \"%s\" already defined\n"
#define ERR_NON_INT_VAR_MAIN "returning non-int variable \"%s\" from main\n"
#define ERR_NON_INT_VAL_MAIN "returning non-int value \"%s\" from main\n"
#define ERR_NON_BOOL_COND "Expected \"%s\" to be of type bool\n"
#define ERR_MAIN_RESERVED "declaration of reserved function \"main\""
#define ERR_NO_REDEFINE_FUNC "cannot redeclare function \"%s\"\n"
#define ERR_ZERO_PARAM_FUNC "passing parameter to function that takes zero parameters\n"
#define ERR_SHADOW_VAR "variable \"%s\" shadows existing variable\n"
#define ERR_EXPECTED_RETURN "expected return value in function \"%s\"\n"
#define ERR_NO_VOID_RETURN "unexpected return from void function \"%s\"\n"
#define ERR_ELSE_MISSING_IF "else statement missing preceding if statement"
#define ERR_UNEXPECTED_CODE_BLOCK "unexpected code block"
#define ERR_UNREACHALE_RETURN "unreachable code after return"
#define ERR_REASSIGN_CONST "cannot reassign const variable \"%s\"\n"
#define ERR_UNASSIGNABLE "unable to assign value to variable \"%s\"\n"
#define ERR_REDUNDANT_REASSIGN "redundant assignment of variable \"%s\" to itself\n"
#define ERR_DIV_BY_ZERO "division by zero"
#define ERR_NO_EXPORT_NESTED "cannot export nested function \"%s\"\n"
#define ERR_NOT_COMPARIBLE "cannot compare value near \"%s\"\n"

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