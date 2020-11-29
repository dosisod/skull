#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/eval/types/types.h"
#include "skull/parse/tokenize.h"

#include "skull/parse/classify.h"

#define TOKEN_TRY_STR(str, type) \
	else if (token_cmp((str), token)) { \
		token->token_type = (type); \
	}

#define TOKEN_SET_IF(cond, type) \
	else if ((cond)) { \
		token->token_type = (type); \
	}

/*
Classify the token `token`.
*/
void classify_token(Token *const token) {
	char32_t *const str = token_str(token);

	if (false) {} // setup for macros

	TOKEN_TRY_STR(U"\n", TOKEN_NEWLINE)
	TOKEN_TRY_STR(U"{", TOKEN_BRACKET_OPEN)
	TOKEN_TRY_STR(U"}", TOKEN_BRACKET_CLOSE)
	TOKEN_TRY_STR(U"(", TOKEN_PAREN_OPEN)
	TOKEN_TRY_STR(U")", TOKEN_PAREN_CLOSE)
	TOKEN_TRY_STR(U"mut", TOKEN_KW_MUT)
	TOKEN_TRY_STR(U"return", TOKEN_KW_RETURN)
	TOKEN_TRY_STR(U"if", TOKEN_KW_IF)
	TOKEN_TRY_STR(U"else", TOKEN_KW_ELSE)
	TOKEN_TRY_STR(U"while", TOKEN_KW_WHILE)
	TOKEN_TRY_STR(U"external", TOKEN_KW_EXTERNAL)
	TOKEN_TRY_STR(U"export", TOKEN_KW_EXPORT)
	TOKEN_TRY_STR(U"=", TOKEN_OPER_EQUAL)
	TOKEN_TRY_STR(U"+", TOKEN_OPER_PLUS)
	TOKEN_TRY_STR(U"-", TOKEN_OPER_MINUS)
	TOKEN_TRY_STR(U"*", TOKEN_OPER_MULT)
	TOKEN_TRY_STR(U"/", TOKEN_OPER_DIV)
	TOKEN_TRY_STR(U":=", TOKEN_OPER_AUTO_EQUAL)
	TOKEN_TRY_STR(U"is", TOKEN_OPER_IS)

	TOKEN_SET_IF(str[0] == '#' && (
		str[1] == ' ' ||
		str[1] == '{' ||
		!str[1]
	), TOKEN_COMMENT)

	TOKEN_SET_IF(is_type_str(str), TOKEN_TYPE)
	TOKEN_SET_IF(is_constant_integer_str(str), TOKEN_INT_CONST)
	TOKEN_SET_IF(is_constant_float_str(str), TOKEN_FLOAT_CONST)
	TOKEN_SET_IF(is_constant_bool_str(str), TOKEN_BOOL_CONST)
	TOKEN_SET_IF(is_constant_rune_str(str), TOKEN_RUNE_CONST)
	TOKEN_SET_IF(is_constant_str_str(str), TOKEN_STR_CONST)

	else if (is_valid_identifier_str(str)) {
		token->token_type = TOKEN_IDENTIFIER;

		if (token->end[-1] == ':') {
			token->token_type = TOKEN_NEW_IDENTIFIER;
			token->end--;

			char32_t *const new_str = token_str(token);

			if (is_type_str(new_str) ||
				is_keyword_str(new_str))
			{
				token->token_type = TOKEN_UNKNOWN;
				token->end++;
			}
			free(new_str);
		}
	}
	free(str);
}

#undef TOKEN_TRY_STR
#undef TOKEN_SET_IF

/*
Classify all tokens pointed to from `token`.
*/
void classify_tokens(Token *head) {
	Token *current = head;

	while (current) {
		classify_token(current);
		current = current->next;
	}
}

/*
Returns true if `name` is a type string.
*/
bool is_type_str(const char32_t *const name) {
	char *const type_name = c32stombs(name);

	const bool is_type = find_type(type_name);
	free(type_name);

	return is_type;
}

/*
Returns true if a `str` is a keyword.
*/
bool is_keyword_str(const char32_t *const str) {
	return (
		c32scmp(U"return", str) ||
		c32scmp(U"mut", str)
	);
}

#define EXHAUST_STR(cond) \
	while (*str) { \
		if (!(cond)) { \
			return false; \
		} \
		str++; \
	}

/*
Returns true if `str` is a valid hex/octal/binary/decimal representation of an integer.

Examples: `-123`, `123`, `0xFF`, `0xff`, `0b1010`, `0o777`
*/
bool is_constant_integer_str(const char32_t *str) {
	if (*str == '0' && str[1] && str[2]) {
		str += 2;

		if (str[-1] == 'x') {
			EXHAUST_STR(c32isxdigit(*str));
		}
		else if (str[-1] == 'b') {
			EXHAUST_STR(*str == '0' || *str == '1');
		}
		else if (str[-1] == 'o') {
			EXHAUST_STR('0' <= *str && *str <= '7');
		}
		else {
			return false;
		}

		return true;
	}

	if (*str == '-') {
		str++;
	}
	if (!*str) {
		return false;
	}

	EXHAUST_STR(c32isdigit(*str));

	return !*str;
}

/*
Returns true if `str` is a valid float (with decimal).

Examples: `123.0`, `-123.0`, `0.0`, `Infinity`
*/
bool is_constant_float_str(const char32_t *str) {
	if (*str == '-') {
		str++;
	}

	if (c32scmp(U"Infinity", str)) {
		return true;
	}

	if (*str == '.') {
		return false;
	}

	while (*str && *str != '.') {
		if (!c32isdigit(*str)) {
			return false;
		}

		str++;
	}

	if (!*str || !str[1]) {
		return false;
	}
	str++;

	EXHAUST_STR(c32isdigit(*str));

	return true;
}

#undef EXHAUST_STR

/*
Returns true if `str` is a valid bool (`true` or `false`).
*/
bool is_constant_bool_str(const char32_t *const str) {
	return c32scmp(U"false", str) || c32scmp(U"true", str);
}

/*
Returns true if `str` is a valid rune.

Examples: `'x'`, `'\n'`, `'\xFF'`, and `' '`.
Won't work: `''`, `'\'`, `'x '`, or `' x'`.
*/
bool is_constant_rune_str(const char32_t *const str) {
	const size_t len = c32slen(str);

	if (*str != '\'' || str[len - 1] != '\'') {
		return false;
	}

	if (4 <= len && len <= 6) {
		return str[1] == '\\';
	}

	return len == 3;
}

/*
Returns true if `str` is a valid string constant.

Examples: `""` and `"hello"`.
*/
bool is_constant_str_str(const char32_t *const str) {
	const size_t len = c32slen(str);

	return (
		len >= 2 &&
		str[0] == '\"' &&
		str[len - 1] == '\"'
	);
}

/*
Returns true if `str` is a valid identifer.
*/
bool is_valid_identifier_str(const char32_t *str) {
	if (*str < 'A' || ('Z' < *str && *str < 'a') || *str > 'z') {
		return false;
	}
	str++;

	while (*str) {
		if (*str == ':') {
			str++;
			break;
		}
		if (!(c32isalnum(*str) ^ (*str == '_'))) {
			return false;
		}
		str++;
	}

	return !*str;
}
