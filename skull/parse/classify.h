#pragma once

#include <uchar.h>

#include "skull/parse/tokenize.h"

enum token_types {
	// default token type
	TOKEN_UNKNOWN,

	TOKEN_IDENTIFIER,
	TOKEN_NEW_IDENTIFIER, // must suffix new var names with ":"

	TOKEN_KW_MUT,
	TOKEN_KW_RETURN,
	TOKEN_KW_IF,

	TOKEN_BRACKET_OPEN,
	TOKEN_BRACKET_CLOSE,

	TOKEN_TYPE,

	TOKEN_OPER_EQUAL,
	TOKEN_OPER_PLUS,
	TOKEN_OPER_MINUS,
	TOKEN_OPER_MULT,
	TOKEN_OPER_DIV,
	TOKEN_OPER_AUTO_EQUAL, // ":="

	TOKEN_INT_CONST,
	TOKEN_FLOAT_CONST,
	TOKEN_BOOL_CONST,
	TOKEN_RUNE_CONST,
	TOKEN_STR_CONST,

	TOKEN_COMMENT,

	TOKEN_END
};

bool is_type_str(const char32_t *);
bool is_keyword_str(const char32_t *);
bool is_constant_integer_str(const char32_t *);
bool is_constant_float_str(const char32_t *);
bool is_constant_bool_str(const char32_t *);
bool is_constant_rune_str(const char32_t *);
bool is_constant_str_str(const char32_t *);
bool is_valid_identifier_str(const char32_t *);

void classify_token(token_t *);
void classify_tokens(token_t *);
