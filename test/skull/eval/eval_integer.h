#pragma once

#include "test/testing.h"

#define TEST_EVAL_INT_CONVERT(str_value, expected_num, expected_error) \
	Token *token=tokenize(str_value); \
	classify_tokens(token); \
	const char32_t *err=0; \
	SkullInt *num=eval_integer(token, &err); \
	const bool pass=( \
		(!num || *num==(expected_num)) && \
		(err==(expected_error) || c32scmp(expected_error, err)) \
	); \
	free(token); \
	free(num); \
	return pass

TEST_DECL(convert_integer_token)
TEST_DECL(convert_negative_integer_token)
TEST_DECL(integer_overflow_returns_error)
TEST_DECL(integer_underflow_returns_error)
TEST_DECL(convert_hex_integer)
TEST_DECL(convert_octal_integer)
TEST_DECL(convert_binary_integer)
TEST_DECL(non_integer_token_fails)

void eval_integer_test_self(_Bool *pass);