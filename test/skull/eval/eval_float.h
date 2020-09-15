#pragma once

#include "test/testing.h"

#define TEST_EVAL_FLOAT_CONVERT(str_value, cond, expected_error) \
	Token *token = tokenize(str_value); \
	classify_tokens(token); \
	const char32_t *err = NULL; \
	SkullFloat *num = eval_float(token, &err); \
	const bool pass = ( \
		(cond) && \
		(err == (expected_error) || c32scmp(expected_error, err)) \
	); \
	free(token); \
	free(num); \
	return pass

TEST_DECL(convert_float_token)
TEST_DECL(convert_negative_float_token)
TEST_DECL(convert_overflow_float)
TEST_DECL(convert_infinity_float_token)
TEST_DECL(convert_neg_infinity_float_token)
TEST_DECL(non_float_token_fails)

void eval_float_test_self(_Bool *);