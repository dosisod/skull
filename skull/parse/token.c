#include <stdbool.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"

#include "skull/parse/token.h"

_Bool is_whitespace(char32_t);
_Bool is_quote(char32_t);

// add current cursor/code state to token
#define SETUP_TOKEN() \
	current->begin = code; \
	current->line = line_num; \
	current->column = column;

#define APPEND_TOKEN() \
	last = current; \
	current->next = make_token(); \
	current = current->next

// pinch end of current token, setup next token
#define PINCH_TOKEN() \
	current->end = code; \
	APPEND_TOKEN(); \
	current->end = code + 1; \
	SETUP_TOKEN()

typedef enum {
	NO_COMMENT,
	LINE_COMMENT,
	BLOCK_COMMENT
} CommentState;

/*
Tokenize `code`, return pointer to first token.
*/
Token *tokenize(const char32_t *code) {
	const char32_t *const code_copy = code;

	Token *const head = make_token();

	Token *current = head;
	Token *last = current;

	unsigned line_num = 1;
	unsigned column = 0;
	char32_t quote = '\0';
	CommentState comment = NO_COMMENT;

	while (*code) {
		column++;

		if (comment) {
			if (comment == LINE_COMMENT && *code == '\n') {
				code--;
				comment = NO_COMMENT;
			}
			else if (comment == BLOCK_COMMENT && *code == '#') {
				code++;

				if (*code == '{') {
					PANIC(ERR_NESTED_BLOCK_COMMENT, {0});
				}

				if (*code == '}') {
					comment = NO_COMMENT;
				}
			}
		}
		else if (!quote && !comment && *code == '#') {
			if (code[1] == ' ') {
				comment = LINE_COMMENT;
			}
			else if (code[1] == '{') {
				comment = BLOCK_COMMENT;
			}
			else {
				PANIC(ERR_INVALID_COMMENT_START, { .i = line_num + 1 });
			}

			if (!current->begin) {
				SETUP_TOKEN();
			}
			else {
				PINCH_TOKEN();
			}
		}
		else if (quote) {
			if (*code == '\\' && (code[1] == '\\' || code[1] == quote)) {
				code++;
			}
			else if (*code == quote) {
				quote = '\0';
			}
		}
		else if (!quote && is_quote(*code)) {
			quote = *code;

			if (!current->begin) {
				SETUP_TOKEN();
			}
		}
		else if (
			*code == '{' ||
			*code == '}' ||
			*code == '(' ||
			*code == ')' ||
			*code == ',' ||
			*code == '\n'
		) {
			if (!current->begin) {
				SETUP_TOKEN();
				current->end = code + 1;
			}
			else {
				PINCH_TOKEN();
			}

			APPEND_TOKEN();
		}
		else if (!current->begin) {
			if (!is_whitespace(*code)) {
				SETUP_TOKEN();
			}
		}
		else if (!current->end) {
			if (is_whitespace(*code)) {
				current->end = code;

				APPEND_TOKEN();
			}
		}

		if (*code == '\n') {
			line_num++;
			column = 0;
		}

		code++;
	}

	if (comment == BLOCK_COMMENT) {
		current->end = code;
		PANIC(ERR_NO_CLOSING_COMMENT, { .tok = current });
	}
	if (quote) {
		current->end = code;
		PANIC(ERR_NO_CLOSING_QUOTE, { .tok = current });
	}

	// close dangling token if there was no whitespace at EOF
	if (current->begin) {
		current->end = code;
	}
	// pop last token since it will not have any data
	else if (current != head) {
		last->next = NULL;
		free(current);
	}
	// there where no tokens to parse, set safe defaults
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
	return c == ' ' ||
		c == '\t' ||
		c == '\r' ||
		c == '\n';
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
	return Calloc(1, sizeof(Token));
}

/*
Make a heap allocated copy of the data inside `token`.

The result of this function must be freed.
*/
char32_t *token_str(const Token *const token) {
	const size_t len = token_len(token);
	char32_t *str;
	str = Malloc((len + 1) * sizeof *str);

	c32sncpy(str, token->begin, len + 1);
	str[len] = '\0';

	return str;
}

/*
Make a heap allocated copy of the data inside `token` as a multi-byte string.

The result of this function must be freed.
*/
char *token_mbs_str(const Token *const token) {
	char32_t *const tmp = token_str(token);
	char *const ret = c32stombs(tmp);

	free(tmp);
	return ret;
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
