#include "test/testing.h"

#define TEST_CLASSIFY_TOKEN(str, expected) \
	const char32_t *code = str; \
	Token *token = tokenize(code); \
	classify_token(token); \
	ASSERT_EQUAL(token->token_type, (expected)); \
	free(token)

#define TEST_CLASSIFY_TOKEN_WITH_LEN(str, tok_type, start_at, end_at) \
	const char32_t *code = (str); \
	Token *t = tokenize(code); \
	classify_tokens(t); \
	ASSERT_EQUAL(t->token_type, (tok_type)); \
	ASSERT_EQUAL(t->begin, code + (start_at)); \
	ASSERT_EQUAL(t->end, code + (end_at)); \
	free(t)

void classifier_test_self(_Bool *pass);
