#include <stdbool.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"

#include "skull/parse/tokenize.h"

//pinch end of current token, setup next token
#define PINCH_TOKEN \
	current->end = code; \
	Token *next_token = make_token(); \
	next_token->begin = code; \
	next_token->end = code + 1; \
	current->next = next_token; \
	current = next_token

/*
Tokenize `code`, return pointer to first token.
*/
Token *tokenize(const char32_t *code) {
	const char32_t * const code_copy = code;

	Token *head = make_token();

	Token *current = head;
	Token *last = current;

	char32_t quote = false;
	bool comment = false;

	while (*code) {
		if (comment) {
			if (*code == '\n') {
				comment = false;
			}
		}
		else if (!comment && code[0] == '#' && code[1] == ' ') {
			comment = true;

			if (!current->begin) {
				current->begin = code;
			}
			else {
				PINCH_TOKEN;
			}
		}
		else if (quote) {
			if (*code == quote) {
				quote = false;
			}
		}
		else if (!quote && is_quote(*code)) {
			quote = *code;

			if (!current->begin) {
				current->begin = code;
			}
		}
		else if (
			*code == '{' ||
			*code == '}' ||
			*code == '(' ||
			*code == ')' ||
			*code == '\n'
		) {
			if (!current->begin) {
				current->begin = code;
				current->end = code + 1;
			}
			else {
				PINCH_TOKEN;
			}

			Token *next_token = make_token();

			last = current;
			current->next = next_token;
			current = next_token;
		}
		else if (!current->begin) {
			if (!is_whitespace(*code)) {
				current->begin = code;
			}
		}
		else if (!current->end) {
			if (is_whitespace(*code)) {
				current->end = code;

				Token *next_token = make_token();

				last = current;
				current->next = next_token;
				current = next_token;
			}
		}
		code++;
	}

	//close dangling token if there was no whitespace at EOF
	if (current->begin) {
		current->end = code;
	}
	//if there is a no token to be created, pop last token
	else if (current != head) {
		last->next = NULL;
		free(current);
	}
	//there where no tokens to parse, set safe defaults
	else {
		head->begin = code_copy;
		head->end = code_copy;
		head->next = NULL;
	}

	return head;
}

/*
Return true if `c` is whitespace.

Whitespace is considered as indent/line related control characters.
*/
__attribute__((const)) bool is_whitespace(char32_t c) {
	return (
		c == ' ' ||
		c == '\t' ||
		c == '\r' ||
		c == '\n'
	);
}

/*
Return true if `c` is a double or single quote.
*/
__attribute__((const)) bool is_quote(char32_t c) {
	return c == '\'' || c == '\"';
}

/*
Allocate and return a token with set defaults.
*/
Token *make_token(void) {
	Token *token;
	token = calloc(1, sizeof *token);
	DIE_IF_MALLOC_FAILS(token);

	return token;
}

/*
Make a heap allocated copy of the data inside `token`.

The result of this function must be freed.
*/
char32_t *token_str(const Token *token) {
	const size_t len = token_len(token);
	char32_t *str;
	str = malloc((len + 1) * sizeof *str);
	DIE_IF_MALLOC_FAILS(str);

	c32sncpy(str, token->begin, len + 1);
	str[len] = '\0';

	return str;
}

/*
Returns true if `str` is equal to the value of `token`.
*/
bool token_cmp(const char32_t *const str, const Token *const token) {
	const size_t len = token_len(token);
	return (
		c32slen(str) == len &&
		c32sncmp(str, token->begin, len)
	);
}

/*
Return the string length of `token`.
*/
__attribute__((pure)) size_t token_len(const Token *const token) {
	return (size_t)(token->end - token->begin);
}

/*
Free all tokens from `head` and beyond.
*/
void free_tokens(Token *head) {
	Token *current;

	while (head) {
		current = head;
		head = head->next;
		current->next = NULL;
		free(current);
	}
}
