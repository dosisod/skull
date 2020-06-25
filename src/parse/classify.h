#pragma once

#include <uchar.h>

#include "tokenize.h"

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

	//operators (add, subtract, assign)
	TOKEN_OPER_EQUAL,
	TOKEN_OPER_PLUS,
	TOKEN_OPER_EQUAL_EQUAL,
	TOKEN_OPER_NOT_EQUAL,
	TOKEN_OPER_AUTO_EQUAL, // ":="

	//constant values, such as ints and strings
	TOKEN_INT_CONST,
	TOKEN_FLOAT_CONST,
	TOKEN_BOOL_CONST,
	TOKEN_CHAR_CONST,
	TOKEN_STR_CONST,

	//cannot be assigned to a token directly
	TOKEN_ANY_NON_BRACKET_TOKEN
};

/*
Returns true if `token` is a type string.
*/
bool is_type_str(const char32_t *name);

/*
Returns true if a `str` is a keyword.
*/
bool is_keyword_str(const char32_t *str);

/*
Returns true if string is a valid hex/octal/binary/decimal integer.

Examples: `-123`, `123`, `0xFF`, `0xff`, `0b1010`, `0o777`
*/
bool is_constant_integer_str(const char32_t *str);

/*
Returns true if string is a valid float (with decimal).

Examples: `123.0`, `-123.0`, `0.0`
*/
bool is_constant_float_str(const char32_t *str);

/*
Returns true if the string is a valid bool (`true` or `false`).
*/
bool is_constant_bool_str(const char32_t *str);

/*
Returns true if the string is a valid char.

Examples: `'x'` and `' '`.
Won't work: `''`, `'x '`, or `' x'`.
*/
bool is_constant_char_str(const char32_t *str);

/*
Returns true if the string is a valid string constant.

Examples: `""` and `"hello"`.
*/
bool is_constant_str_str(const char32_t *str);

/*
Returns true the passed character the start of a valid identifier.
*/
bool is_valid_identifier_str(const char32_t *str);

/*
Classify the token `token`.
*/
void classify_token(token_t *token);

/*
Starting at token `token`, go through and classify each token in linked list.
*/
void classify_tokens(token_t *head);
