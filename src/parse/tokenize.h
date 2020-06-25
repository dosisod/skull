#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "../common/str.h"

typedef struct token_t {
	const char32_t *begin;
	const char32_t *end;
	uint8_t token_type;

	struct token_t *next;
} token_t;

/*
Return true if `c` is whitespace.

Whitespace includes control-characters, non-printable characters, and spaces.
*/
bool is_whitespace(char32_t c);

/*
Return true if `c` is a double or single quote.
*/
bool is_quote(char32_t c);

/*
Allocate and return a token with set defaults.
*/
token_t *make_token(void);

/*
Tokenize the passed code, returning the head to a linked list of tokens.
*/
token_t *tokenize(const char32_t *code);

/*
Free all the tokens from a linked list of tokens.

This function should be called at the head of the linked list.
*/
void free_tokens(token_t *head);

/*
Returns the character length of the passed token
*/
size_t token_len(const token_t *token);

/*
Returns true if `str` is equal to the value of `token`.
*/
bool token_cmp(const char32_t *str, const token_t *token);

/*
Return a copy of the string data inside `token`.
*/
char32_t *token_str(const token_t *token);

#define MAKE_TOKEN_BUF(buf, token) \
	const size_t buf##_len=token_len(token); \
	char32_t buf[buf##_len + 1]; \
	c32slcpy(buf, (token)->begin, buf##_len + 1)
