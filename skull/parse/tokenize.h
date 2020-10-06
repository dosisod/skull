#pragma once

#include <uchar.h>

typedef struct Token Token;

typedef struct Token {
	const char32_t *begin;
	const char32_t *end;
	unsigned token_type;

	Token *next;
} Token;

_Bool is_whitespace(char32_t);
_Bool is_quote(char32_t);

Token *make_token(void);

Token *tokenize(const char32_t *);

void free_tokens(Token *);

size_t token_len(const Token *);
_Bool token_cmp(const char32_t *, const Token *);

char32_t *token_str(const Token *);
