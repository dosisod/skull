#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "skull/common/str.h"

typedef struct Token {
	const char32_t *begin;
	const char32_t *end;
	unsigned token_type;

	struct Token *next;
} Token;

bool is_whitespace(char32_t);
bool is_quote(char32_t);

Token *make_token(void);

Token *tokenize(const char32_t *);

void free_tokens(Token *);

size_t token_len(const Token *);
bool token_cmp(const char32_t *, const Token *);

char32_t *token_str(const Token *);
