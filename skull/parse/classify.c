#include <stddef.h>
#include <stdlib.h>

#include "skull/common/str.h"
#include "skull/common/wegex.h"
#include "skull/eval/function.h"
#include "skull/eval/types/types.h"
#include "skull/parse/constants.h"
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
void classify_token(token_t *token) {
	char32_t *str = token_str(token);

	if (false) {} // setup for macros

	TOKEN_TRY_STR(U"[", TOKEN_BRACKET_OPEN)
	TOKEN_TRY_STR(U"]", TOKEN_BRACKET_CLOSE)
	TOKEN_TRY_STR(U"mut", TOKEN_KW_MUT)
	TOKEN_TRY_STR(U"return", TOKEN_KW_RETURN)
	TOKEN_TRY_STR(U"=", TOKEN_OPER_EQUAL)
	TOKEN_TRY_STR(U"+", TOKEN_OPER_PLUS)
	TOKEN_TRY_STR(U"-", TOKEN_OPER_MINUS)
	TOKEN_TRY_STR(U"*", TOKEN_OPER_MULT)
	TOKEN_TRY_STR(U"/", TOKEN_OPER_DIV)
	TOKEN_TRY_STR(U":=", TOKEN_OPER_AUTO_EQUAL)

	TOKEN_SET_IF(c32sncmp(str, LINE_COMMENT, LINE_COMMENT_LEN), TOKEN_COMMENT)

	TOKEN_SET_IF(is_type_str(str), TOKEN_TYPE)
	TOKEN_SET_IF(is_constant_integer_str(str), TOKEN_INT_CONST)
	TOKEN_SET_IF(is_constant_float_str(str), TOKEN_FLOAT_CONST)
	TOKEN_SET_IF(is_constant_bool_str(str), TOKEN_BOOL_CONST)

	else if (is_constant_char_str(str)) {
		token->token_type = TOKEN_CHAR_CONST;

		//dont include ''s as part of string
		token->begin++;
		token->end--;
	}
	else if (is_constant_str_str(str)) {
		token->token_type = TOKEN_STR_CONST;

		//dont include ""s as part of string
		token->begin++;
		token->end--;
	}
	else if (is_valid_identifier_str(str)) {
		token->token_type = TOKEN_IDENTIFIER;

		if (*(token->end - 1) == U':') {
			token->token_type = TOKEN_NEW_IDENTIFIER;
			token->end--;

			char32_t *new_str = token_str(token);

			if (is_type_str(new_str) ||
				is_keyword_str(new_str) ||
				is_func_name_str(new_str))
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
void classify_tokens(token_t *head) {
	token_t *current = head;

	while (current) {
		classify_token(current);
		current = current->next;
	}
}

/*
Returns true if `name` is a type string.
*/
bool is_type_str(const char32_t *name) {
	type_t *current = TYPES_AVAILABLE;
	while (current) {
		if (c32scmp(current->name, name)) {
			return true;
		}
		current = current->next;
	}
	return false;
}

/*
Returns true if a `str` is a keyword.
*/
bool is_keyword_str(const char32_t *str) {
	return (
		c32scmp(U"return", str) ||
		c32scmp(U"mut", str)
	);
}

/*
Returns true if `str` is a valid hex/octal/binary/decimal representation of an integer.

Examples: `-123`, `123`, `0xFF`, `0xff`, `0b1010`, `0o777`
*/
bool is_constant_integer_str(const char32_t *str) {
	return (
		wegex_match(U"?-+\n", str) ||
		wegex_match(U"0x+\b", str) ||
		wegex_match(U"0b+[01]", str) ||
		wegex_match(U"0o+[01234567]", str)
	);
}

/*
Returns true if `str` is a valid float (with decimal).

Examples: `123.0`, `-123.0`, `0.0`, `Infinity`
*/
bool is_constant_float_str(const char32_t *str) {
	return (
		wegex_match(U"?-+\n.+\n", str) ||
		wegex_match(U"?-Infinity", str)
	);
}

/*
Returns true if `str` is a valid bool (`true` or `false`).
*/
bool is_constant_bool_str(const char32_t *str) {
	return c32scmp(U"false", str) || c32scmp(U"true", str);
}

/*
Returns true if `str` is a valid char.

Examples: `'x'` and `' '`.
Won't work: `''`, `'x '`, or `' x'`.
*/
bool is_constant_char_str(const char32_t *str) {
	return (
		c32slen(str) == 3 &&
		str[0] == U'\'' &&
		str[2] == U'\''
	);
}

/*
Returns true if `str` is a valid string constant.

Examples: `""` and `"hello"`.
*/
bool is_constant_str_str(const char32_t *str) {
	const size_t len = c32slen(str);

	return (
		len >= 2 &&
		str[0] == U'\"' &&
		str[len - 1] == U'\"'
	);
}

/*
Returns true if `str` is a valid identifer.
*/
bool is_valid_identifier_str(const char32_t *str) {
	return wegex_match(U"\a*[\f_]?:", str);
}
