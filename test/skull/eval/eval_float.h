#pragma once

#include "test/testing.h"

#define TEST_EVAL_FLOAT_CONVERT(str_value, cond, expected_error) \
	Token *token = tokenize(str_value); \
	classify_tokens(token); \
	const char32_t *err = NULL; \
	SkullFloat *num = eval_float(token, &err); \
	ASSERT_TRUTHY(cond); \
	ASSERT_TRUTHY(c32scmp( \
		(_Bool)(expected_error) ? (expected_error) : U"", \
		(_Bool)(err) ? (err) : U"") \
	); \
	free(token); \
	free(num)

TEST_DECL(convert_float_token)
TEST_DECL(convert_negative_float_token)
TEST_DECL(convert_overflow_float)
TEST_DECL(convert_infinity_float_token)
TEST_DECL(convert_neg_infinity_float_token)
TEST_DECL(non_float_token_fails)

void eval_float_test_self(_Bool *);