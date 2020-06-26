#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#include "../../src/errors.h"
#include "../../src/eval/eval_integer.h"
#include "../../src/parse/classify.h"
#include "../../test/testing.h"

#define TEST_EVAL_INT_CONVERT(str_value, expected_num, expected_error) \
	token_t *token=tokenize(str_value); \
	classify_tokens(token); \
	const char32_t *err=0; \
	int64_t num=eval_integer(token, &err); \
	const bool pass=( \
		num==(expected_num) && \
		err==(expected_error) \
	); \
	free(token); \
	return pass;

TEST(convert_integer_token, {
	TEST_EVAL_INT_CONVERT(U"1234", 1234, NULL);
})

TEST(convert_negative_integer_token, {
	TEST_EVAL_INT_CONVERT(U"-1234", -1234, NULL);
})

TEST(integer_overflow_returns_error, {
	TEST_EVAL_INT_CONVERT(U"9999999999999999999", LLONG_MAX, ERR_OVERFLOW);
})

TEST(integer_underflow_returns_error, {
	TEST_EVAL_INT_CONVERT(U"-9999999999999999999", LLONG_MIN, ERR_OVERFLOW);
})

TEST(convert_hex_integer, {
	TEST_EVAL_INT_CONVERT(U"0xff", 255, NULL);
})

TEST(convert_octal_integer, {
	TEST_EVAL_INT_CONVERT(U"0o777", 0777, NULL);
})

TEST(convert_binary_integer, {
	TEST_EVAL_INT_CONVERT(U"0b1111", 15, NULL);
})

TEST(non_integer_token_fails, {
	TEST_EVAL_INT_CONVERT(U"not_an_int", 0, ERR_TYPE_MISMATCH);
})

#undef TEST_EVAL_INT_CONVERT

void eval_integer_test_self(bool *pass) {
	tests_t tests={
		test_convert_integer_token,
		test_convert_negative_integer_token,
		test_integer_overflow_returns_error,
		test_integer_underflow_returns_error,
		test_convert_hex_integer,
		test_convert_octal_integer,
		test_convert_binary_integer,
		test_non_integer_token_fails,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}