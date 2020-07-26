#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "skull/common/str.h"

typedef struct token_t {
	const char32_t *begin;
	const char32_t *end;
	uint8_t token_type;

	struct token_t *next;
} token_t;

bool is_whitespace(char32_t);
bool is_quote(char32_t);

token_t *make_token(void);

token_t *tokenize(const char32_t *);

void free_tokens(token_t *);

size_t token_len(const token_t *);
bool token_cmp(const char32_t *, const token_t *);

char32_t *token_str(const token_t *);

#define MAKE_TOKEN_BUF(buf, token) \
	const size_t buf##_len=token_len(token); \
	char32_t buf[buf##_len + 1]; \
	c32slcpy(buf, (token)->begin, buf##_len + 1)
