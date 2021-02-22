#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/eval/types/types.h"

#include "skull/parse/classify.h"

#define TOKEN_TRY_STR(str, token_type) \
	else if (token_cmp(U##str, token)) { \
		token->type = (token_type); \
	}

#define TOKEN_SET_IF(cond, token_type) \
	else if ((cond)) { \
		token->type = (token_type); \
	}

/*
Classify the token `token`.
*/
void classify_token(Token *const token) {
	char32_t *const str = token_str(token);

	if (false) {} // setup for macros

	TOKEN_TRY_STR("\n", TOKEN_NEWLINE)
	TOKEN_TRY_STR(",", TOKEN_COMMA)
	TOKEN_TRY_STR("{", TOKEN_BRACKET_OPEN)
	TOKEN_TRY_STR("}", TOKEN_BRACKET_CLOSE)
	TOKEN_TRY_STR("(", TOKEN_PAREN_OPEN)
	TOKEN_TRY_STR(")", TOKEN_PAREN_CLOSE)
	TOKEN_TRY_STR("mut", TOKEN_KW_MUT)
	TOKEN_TRY_STR("return", TOKEN_KW_RETURN)
	TOKEN_TRY_STR("unreachable", TOKEN_KW_UNREACHABLE)
	TOKEN_TRY_STR("if", TOKEN_KW_IF)
	TOKEN_TRY_STR("elif", TOKEN_KW_ELIF)
	TOKEN_TRY_STR("else", TOKEN_KW_ELSE)
	TOKEN_TRY_STR("while", TOKEN_KW_WHILE)
	TOKEN_TRY_STR("external", TOKEN_KW_EXTERNAL)
	TOKEN_TRY_STR("export", TOKEN_KW_EXPORT)
	TOKEN_TRY_STR("=", TOKEN_OPER_EQUAL)
	TOKEN_TRY_STR("+", TOKEN_OPER_PLUS)
	TOKEN_TRY_STR("-", TOKEN_OPER_MINUS)
	TOKEN_TRY_STR("*", TOKEN_OPER_MULT)
	TOKEN_TRY_STR("/", TOKEN_OPER_DIV)
	TOKEN_TRY_STR("mod", TOKEN_OPER_MOD)
	TOKEN_TRY_STR(":=", TOKEN_OPER_AUTO_EQUAL)
	TOKEN_TRY_STR("not", TOKEN_OPER_NOT)
	TOKEN_TRY_STR("is", TOKEN_OPER_IS)
	TOKEN_TRY_STR("isnt", TOKEN_OPER_ISNT)
	TOKEN_TRY_STR("and", TOKEN_OPER_AND)
	TOKEN_TRY_STR("or", TOKEN_OPER_OR)
	TOKEN_TRY_STR("xor", TOKEN_OPER_XOR)
	TOKEN_TRY_STR("<", TOKEN_OPER_LESS_THAN)
	TOKEN_TRY_STR(">", TOKEN_OPER_GTR_THAN)
	TOKEN_TRY_STR("<=", TOKEN_OPER_LESS_THAN_EQ)
	TOKEN_TRY_STR(">=", TOKEN_OPER_GTR_THAN_EQ)
	TOKEN_TRY_STR("<<", TOKEN_OPER_LSHIFT)
	TOKEN_TRY_STR(">>", TOKEN_OPER_RSHIFT)
	TOKEN_TRY_STR("^", TOKEN_OPER_POW)

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
		token->type = TOKEN_IDENTIFIER;

		if (token->end[-1] == ':') {
			token->type = TOKEN_NEW_IDENTIFIER;

			const size_t len = token_len(token);
			str[len - 1] = '\0';

			if (is_type_str(str) || is_reserved_str(str)) {
				token->type = TOKEN_UNKNOWN;
			}
			else {
				token->end--;
			}
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
	while (head) {
		classify_token(head);
		head = head->next;
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
Returns true if a `str` is a reserved.
*/
bool is_reserved_str(const char32_t *const str) {
	return c32scmp(U"return", str) ||
		c32scmp(U"mut", str) ||
		c32scmp(U"not", str) ||
		c32scmp(U"is", str);
}

#define EXHAUST_STR(cond) \
	while (*str) { \
		if (!(cond)) { \
			return false; \
		} \
		str++; \
	}

/*
Returns true if `str` is a valid hex/octal/binary/decimal representation
of an integer.

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
