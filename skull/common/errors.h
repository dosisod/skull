#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <uchar.h>

#include "skull/parse/token.h"

typedef enum {
	ERR_UNEXPECTED_TOKEN = 1000,
	ERR_EOF_NO_BRACKET = 1001,
	ERR_TYPE_MISMATCH = 1002,
	ERR_FUNC_TYPE_MISMATCH = 1003,
	ERR_EXPECTED_SAME_TYPE = 1004,
	ERR_ASSIGN_BAD_TYPE = 1005,
	ERR_BAD_ESCAPE = 1006,
	ERR_VAR_NOT_FOUND = 1007,
	ERR_OVERFLOW = 1008,
	ERR_MISSING_OPEN_BRAK = 1009,
	ERR_MISSING_CLOSING_PAREN = 1010,
	ERR_MISSING_DECLARATION = 1011,
	ERR_MISSING_BLOCK = 1012,
	ERR_NO_CLOSING_COMMENT = 1013,
	ERR_NO_CLOSING_QUOTE = 1014,
	ERR_NO_VOID_ASSIGN = 1015,
	ERR_INVALID_INPUT = 1016,
	ERR_VAR_ALREADY_DEFINED = 1017,
	ERR_NON_INT_MAIN = 1018,
	ERR_NON_BOOL_EXPR = 1019,
	ERR_MAIN_RESERVED = 1020,
	ERR_NO_REDEFINE_FUNC = 1021,
	ERR_NO_REDEFINE_VAR_AS_FUNC = 1022,
	ERR_NO_REDEFINE_FUNC_AS_VAR = 1023,
	ERR_ZERO_PARAM_FUNC = 1024,
	ERR_SHADOW_VAR = 1025,
	ERR_EXPECTED_RETURN = 1026,
	ERR_NO_VOID_RETURN = 1027,
	ERR_ELSE_MISSING_IF = 1028,
	ERR_UNEXPECTED_CODE_BLOCK = 1029,
	ERR_EMPTY_BLOCK = 1030,
	ERR_UNREACHABLE_CODE = 1031,
	ERR_REASSIGN_CONST = 1032,
	ERR_INVALID_EXPR = 1033,
	ERR_NO_DANGLING_EXPR = 1034,
	ERR_REDUNDANT_REASSIGN = 1035,
	ERR_DIV_BY_ZERO = 1036,
	ERR_NO_NESTED = 1037,
	ERR_TOP_LVL_ALIAS_ONLY = 1038,
	ERR_NOT_COMPARIBLE = 1039,
	ERR_EXPECTED_COMMA = 1040,
	ERR_UNCLOSED_FUNC_CALL = 1041,
	ERR_TYPE_NOT_FOUND = 1042,
	ERR_ALIAS_ALREADY_DEFINED = 1043,
	ERR_ASSIGN_MISSING_EXPR = 1044,
	ERR_RETURN_MISSING_EXPR = 1045,
	ERR_EXPECTED_EXPR = 1046,
	ERR_NO_CONTROL_CHAR = 1047,
	ERR_INVALID_NUM_PARAMS = 1048,
	ERR_INVALID_COMMENT_START = 1049,
	ERR_ILLEGAL_SEQ_AT = 1050,
	ERR_ILLEGAL_SEQ = 1051,
	ERR_POW_BAD_TYPE = 1052,
	ERR_EXPECTED_NEWLINE = 1053,
	WARN_VAR_NOT_CONST = 1054,
	WARN_VAR_UNUSED = 1055,
	WARN_FUNC_UNUSED = 1056,
	WARN_COND_ALWAYS_TRUE = 1057,
	WARN_COND_ALWAYS_FALSE = 1058,
	WARN_FILE_EMPTY = 1059,
	WARN_NESTED_BLOCK_COMMENT = 1060,
	WARN_TRIVIAL_TYPE = 1061
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

#define FMT_ERROR(fmt, ...) {\
	char *_fmt_error_str = fmt_error(ERROR_FATAL, fmt, (ErrorMsg[]){ \
		__VA_ARGS__, \
		{0} \
	}); \
	fprintf(stderr, "%s", _fmt_error_str); \
	free(_fmt_error_str); \
}

char *fmt_error(ErrorType, ErrorCode, ErrorMsg []);

void fmt_error_stringify(ErrorMsg *const);
