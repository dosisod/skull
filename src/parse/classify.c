#include <stddef.h>

#include "../../src/common/str.h"
#include "../../src/common/wegex.h"
#include "tokenize.h"
#include "types.h"

#include "classify.h"

/*
Returns true if `name` is a type string.
*/
bool is_type_str(const char32_t *name) {
	type_t *current=TYPES_AVAILABLE;
	while (current) {
		if (c32scmp(current->name, name)) {
			return true;
		}
		current=current->next;
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

Examples: `123.0`, `-123.0`, `0.0`
*/
bool is_constant_float_str(const char32_t *str) {
	return wegex_match(U"?-+\n.+\n", str);
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
	return c32slen(str)==3 && str[0]=='\'' && str[2]=='\'';
}

/*
Returns true if `str` is a valid string constant.

Examples: `""` and `"hello"`.
*/
bool is_constant_str_str(const char32_t *str) {
	const size_t len=c32slen(str);

	return len>=2 && str[0]==U'\"' && str[len - 1]==U'\"';
}

/*
Returns true if `str` is a valid identifer.
*/
bool is_valid_identifier_str(const char32_t *str) {
	return wegex_match(U"\a*[\f_]?:", str);
}

/*
Classify the token `token`.
*/
void classify_token(token_t *token) {
	MAKE_TOKEN_BUF(buf, token);

	if (token_cmp(U"[", token)) {
		token->token_type=TOKEN_BRACKET_OPEN;
	}
	else if (token_cmp(U"]", token)) {
		token->token_type=TOKEN_BRACKET_CLOSE;
	}
	else if (token_cmp(U"\n", token)) {
		token->token_type=TOKEN_NEWLINE;
	}
	else if (token_cmp(U",", token)) {
		token->token_type=TOKEN_COMMA;
	}
	else if (token_cmp(U"mut", token)) {
		token->token_type=TOKEN_KW_MUT;
	}
	else if (token_cmp(U"return", token)) {
		token->token_type=TOKEN_KW_RETURN;
	}
	else if (token_cmp(U"or", token)) {
		token->token_type=TOKEN_KW_OR;
	}
	else if (token_cmp(U"and", token)) {
		token->token_type=TOKEN_KW_AND;
	}
	else if (token_cmp(U"not", token)) {
		token->token_type=TOKEN_KW_NOT;
	}
	else if (token_cmp(U"=", token)) {
		token->token_type=TOKEN_OPER_EQUAL;
	}
	else if (token_cmp(U"+", token)) {
		token->token_type=TOKEN_OPER_PLUS;
	}
	else if (token_cmp(U"==", token)) {
		token->token_type=TOKEN_OPER_EQUAL_EQUAL;
	}
	else if (token_cmp(U"!=", token)) {
		token->token_type=TOKEN_OPER_NOT_EQUAL;
	}
	else if (token_cmp(U":=", token)) {
		token->token_type=TOKEN_OPER_AUTO_EQUAL;
	}
	else if (token_cmp(U"->", token)) {
		token->token_type=TOKEN_FUNCTION;
	}
	else if (is_type_str(buf)) {
		token->token_type=TOKEN_TYPE;
	}
	else if (is_constant_integer_str(buf)) {
		token->token_type=TOKEN_INT_CONST;
	}
	else if (is_constant_float_str(buf)) {
		token->token_type=TOKEN_FLOAT_CONST;
	}
	else if (is_constant_bool_str(buf)) {
		token->token_type=TOKEN_BOOL_CONST;
	}
	else if (is_constant_char_str(buf)) {
		token->token_type=TOKEN_CHAR_CONST;

		//dont include ''s as part of string
		token->begin++;
		token->end--;
	}
	else if (is_constant_str_str(buf)) {
		token->token_type=TOKEN_STR_CONST;

		//dont include ""s as part of string
		token->begin++;
		token->end--;
	}
	else if (is_valid_identifier_str(buf)) {
		token->token_type=TOKEN_IDENTIFIER;

		if (*(token->end - 1)==U':') {
			token->token_type=TOKEN_NEW_IDENTIFIER;
			token->end--;

			MAKE_TOKEN_BUF(tmp_buf, token);
			if (is_type_str(tmp_buf) || is_keyword_str(tmp_buf)) { // NOLINT
				token->token_type=TOKEN_UNKNOWN;
				token->end++;
			}
		}
	}
}

/*
Classify all tokens pointed to from `token`.
*/
void classify_tokens(token_t *head) {
	token_t *current=head;

	while (current!=NULL) {
		classify_token(current);
		current=current->next;
	}
}