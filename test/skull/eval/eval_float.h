#pragma once

#include "test/testing.h"

#define TEST_EVAL_FLOAT_CONVERT(str_value, cond, expected_error) \
	Token *token = tokenize(str_value); \
	classify_tokens(token); \
	char32_t *err = NULL; \
	SkullFloat *num = eval_float(token, &err); \
	ASSERT_TRUTHY(cond); \
	ASSERT_TRUTHY(c32scmp( \
		(_Bool)(expected_error) ? (expected_error) : U"", \
		(_Bool)(err) ? (err) : U"") \
	); \
	free(token); \
	free(num)

void eval_float_test_self(_Bool *);