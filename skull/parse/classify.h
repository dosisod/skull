#pragma once

#include <uchar.h>

#include "skull/parse/tokenize.h"

enum token_types {
	//default token type
	TOKEN_UNKNOWN,

	//identifiers are things like variable and function names
	TOKEN_IDENTIFIER,
	TOKEN_NEW_IDENTIFIER, //must suffix new var names with ":"

	//indicates the start of a function
	TOKEN_FUNCTION,

	//keyword related tokens
	TOKEN_KW_MUT,
	TOKEN_KW_RETURN,
	TOKEN_KW_OR,
	TOKEN_KW_AND,
	TOKEN_KW_NOT,

	TOKEN_BRACKET_OPEN,
	TOKEN_BRACKET_CLOSE,

	TOKEN_NEWLINE,
	TOKEN_COMMA,

	TOKEN_TYPE,

	TOKEN_OPER_EQUAL,
	TOKEN_OPER_PLUS,
	TOKEN_OPER_MINUS,
	TOKEN_OPER_MULT,
	TOKEN_OPER_DIV,
	TOKEN_OPER_EQUAL_EQUAL,
	TOKEN_OPER_NOT_EQUAL,
	TOKEN_OPER_AUTO_EQUAL, // ":="

	TOKEN_INT_CONST,
	TOKEN_FLOAT_CONST,
	TOKEN_BOOL_CONST,
	TOKEN_CHAR_CONST,
	TOKEN_STR_CONST,

	TOKEN_COMMENT,

	//cannot be assigned to a token directly
	TOKEN_ANY_NON_BRACKET_TOKEN
};

bool is_type_str(const char32_t *);
bool is_keyword_str(const char32_t *);
bool is_constant_integer_str(const char32_t *);
bool is_constant_float_str(const char32_t *);
bool is_constant_bool_str(const char32_t *);
bool is_constant_char_str(const char32_t *);
bool is_constant_str_str(const char32_t *);
bool is_valid_identifier_str(const char32_t *);

void classify_token(token_t *);
void classify_tokens(token_t *);
