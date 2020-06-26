#include <stddef.h>

#include "../../src/common/str.h"
#include "../../src/common/wegex.h"
#include "tokenize.h"
#include "types.h"

#include "classify.h"

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

bool is_keyword_str(const char32_t *str) {
	return (
		c32scmp(U"return", str) ||
		c32scmp(U"mut", str)
	);
}

bool is_constant_integer_str(const char32_t *str) {
	return (
		wegex_match(U"?-+\n", str) ||
		wegex_match(U"0x+\b", str) ||
		wegex_match(U"0b+[01]", str) ||
		wegex_match(U"0o+[01234567]", str)
	);
}

bool is_constant_float_str(const char32_t *str) {
	return wegex_match(U"?-+\n.+\n", str);
}

bool is_constant_bool_str(const char32_t *str) {
	return c32scmp(U"false", str) || c32scmp(U"true", str);
}

bool is_constant_char_str(const char32_t *str) {
	return c32slen(str)==3 && str[0]=='\'' && str[2]=='\'';
}

bool is_constant_str_str(const char32_t *str) {
	const size_t len=c32slen(str);

	return len>=2 && str[0]==U'\"' && str[len - 1]==U'\"';
}

bool is_valid_identifier_str(const char32_t *str) {
	return wegex_match(U"\a*[\f_]?:", str);
}

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
			if (is_type_str(tmp_buf) || is_keyword_str(tmp_buf)) {
				token->token_type=TOKEN_UNKNOWN;
				token->end++;
			}
		}
	}
}

void classify_tokens(token_t *head) {
	token_t *current=head;

	while (current!=NULL) {
		classify_token(current);
		current=current->next;
	}
}
