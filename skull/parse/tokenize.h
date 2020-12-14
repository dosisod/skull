#pragma once

#include <uchar.h>

typedef struct Token Token;

/*
Stores the smallest meaningful chunk of parsed code.

`begin` points to the first character in the token.

`end` points to the last character in the token.

`token_type` stores the type of token.

`line` stores the line that the token starts at.

`column` stores the column that the token start of the token is in.

`next` stores the next parsed token.
*/
typedef struct Token {
	const char32_t *begin;
	const char32_t *end;

	unsigned token_type;
	unsigned line;
	unsigned column;

	Token *next;
} Token;

_Bool is_whitespace(char32_t);
_Bool is_quote(char32_t);

Token *make_token(void);

Token *tokenize(const char32_t *);

void free_tokens(Token *const);

size_t token_len(const Token *);
_Bool token_cmp(const char32_t *const, const Token *const);

char32_t *token_str(const Token *);
char *token_mbs_str(const Token *);
