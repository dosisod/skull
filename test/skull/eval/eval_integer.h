#pragma once

#include "test/testing.h"

#define TEST_EVAL_INT_CONVERT(str_value, expected_num, expected_error) \
	Token *token = tokenize(str_value); \
	classify_tokens(token); \
	char32_t *err = 0; \
	SkullInt num = eval_integer(token, &err); \
	ASSERT_TRUTHY(num == (expected_num)); \
	ASSERT_TRUTHY(c32scmp( \
		(_Bool)(expected_error) ? (expected_error) : U"", \
		(_Bool)(err) ? (err) : U"") \
	); \
	free(token); \
	free(err)

void eval_integer_test_self(_Bool *pass);