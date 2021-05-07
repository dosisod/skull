#include <stdbool.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"

#include "skull/parse/token.h"

bool is_whitespace(char32_t);
bool is_quote(char32_t);

static bool iter_comment(Token *, const char32_t **, unsigned *, unsigned *);
static bool iter_quote(Token *, const char32_t **, unsigned *, unsigned *);

typedef enum {
	NO_COMMENT,
	LINE_COMMENT,
	BLOCK_COMMENT
} CommentState;

/*
Allocate and append next token, return newly created token.
*/
Token *setup_next(Token *token) {
	return (token->next = make_token());
}

/*
Tokenize `code` into linked list of tokens.
*/
Token *tokenize(const char32_t *code) {
	Token *const head = make_token();

	Token *token = head;
	Token *last = token;

	unsigned line_num = 1;
	unsigned column = 0;

	while (*code) {
		column++;

		if (*code == '#') {
			if (iter_comment(token, &code, &line_num, &column)) {
				return NULL;
			}

			token->type = TOKEN_COMMENT;

			if (!*code) break;
		}
		else if (is_quote(*code)) {
			if (iter_quote(token, &code, &line_num, &column)) {
				free_tokens(head);
				return NULL;
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
			if (token->begin) {
				token->end = code;
				token = setup_next(token);
			}

			*token = (Token){
				.begin = code,
				.end = code + 1,
				.location = { .line = line_num, .column = column }
			};

			switch (*code) {
				case '{': token->type = TOKEN_BRACKET_OPEN; break;
				case '}': token->type = TOKEN_BRACKET_CLOSE; break;
				case '(': token->type = TOKEN_PAREN_OPEN; break;
				case ')': token->type = TOKEN_PAREN_CLOSE; break;
				case ',': token->type = TOKEN_COMMA; break;
				case '\n': token->type = TOKEN_NEWLINE; break;
				default: break; // make GCC happy
			}

			last = token;
			token = setup_next(token);
		}
		else if (!token->begin) {
			if (!is_whitespace(*code)) {
				token->begin = code;
				token->location.line = line_num;
				token->location.column = column;
			}
		}
		else if (!token->end) {
			if (is_whitespace(*code)) {
				token->end = code;
				last = token;
				token = setup_next(token);
			}
		}

		if (*code == '\n') {
			line_num++;
			column = 0;
		}

		code++;
	}

	// close dangling token if there was no whitespace at EOF
	if (token->begin) {
		token->end = code;
	}
	// pop last token since it will not have any data
	else if (token != head) {
		last->next = NULL;
		free(token);
	}

	return head;
}

/*
Iterate through comment, starting at `code`.

Return `true` if errors occurred.
*/
static bool iter_comment(
	Token *token,
	const char32_t **_code,
	unsigned *line_num,
	unsigned *column
) {
	const char32_t *code = *_code;

	CommentState comment = NO_COMMENT;

	if (code[1] == ' ')
		comment = LINE_COMMENT;

	else if (code[1] == '{')
		comment = BLOCK_COMMENT;

	else {
		Location location = (Location){ .line = *line_num, .column = *column };
		FMT_ERROR(ERR_INVALID_COMMENT_START, { .loc = &location });
		return true;
	}

	if (!token->begin) {
		token->begin = code;
		token->location.line = *line_num;
		token->location.column = *column;
	}

	code++;
	(*column)++;

	do {
		code++;
		(*column)++;

		if (comment == LINE_COMMENT && *code == '\n') {
			code--;
			break;
		}
		if (comment == BLOCK_COMMENT && *code == '#') {
			code++;
			(*column)++;

			if (*code == '}') break;

			if (*code == '\n') {
				(*line_num)++;
				*column = 0;
			}

			else if (*code == '{') {
				Location location = (Location){
					.line = *line_num,
					.column = *column
				};
				FMT_ERROR(ERR_NESTED_BLOCK_COMMENT, { .loc = &location });
				return true;
			}
		}
	} while (*code);

	*_code = code;

	if (!*code && comment == BLOCK_COMMENT) {
		FMT_ERROR(ERR_NO_CLOSING_COMMENT, { .loc = &token->location });
		return true;
	}

	return false;
}

/*
Iterate through a quote, starting at `code`.

Return `true` if errors occurred.
*/
static bool iter_quote(
	Token *token,
	const char32_t **_code,
	unsigned *line_num,
	unsigned *column
) {
	const char32_t *code = *_code;

	char32_t quote = *code;

	if (!token->begin) {
		token->begin = code;
		token->location.line = *line_num;
		token->location.column = *column;
	}

	do {
		code++;

		if (*code == '\n') {
			(*line_num)++;
			*column = 0;
		}

		if (*code == '\\' && (code[1] == '\\' || code[1] == quote)) {
			code++;
			(*column)++;
		}

		else if (*code == quote) break;

	} while (*code);

	if (!*code) {
		FMT_ERROR(ERR_NO_CLOSING_QUOTE, { .loc = &token->location });
		return true;
	}

	*_code = code;
	return false;
}

/*
Return true if `c` is whitespace.

Whitespace is considered as indent/line related control characters.
*/
__attribute__((const)) bool is_whitespace(char32_t c) {
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
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
	char *const ret = c32stombs(tmp, &token->location);

	free(tmp);
	return ret;
}

/*
Returns true if `str` is equal to the value of `token`.
*/
bool token_cmp(const char32_t *const str, const Token *const token) {
	const size_t len = token_len(token);

	return c32slen(str) == len &&
		c32sncmp(str, token->begin, len);
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
	Token *token;

	while (head) {
		token = head;
		head = head->next;
		token->next = NULL;
		free(token);
	}
}
