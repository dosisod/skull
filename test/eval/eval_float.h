#include <stdbool.h>
#include <stdint.h>

#include "../../src/errors.h"
#include "../../src/eval/eval_float.h"
#include "../../src/parse/classify.h"
#include "../../test/testing.h"

#define TEST_EVAL_FLOAT_CONVERT(str_value, expected_val, expected_error) \
	token_t *token=tokenize(str_value); \
	classify_tokens(token); \
	const char32_t *err=NULL; \
	long double num=eval_float(token, &err); \
	const bool pass=( \
		(int)num==(int)(expected_val) && \
		err==(expected_error) \
	); \
	free(token); \
	return pass;

TEST(convert_float_token, {
	TEST_EVAL_FLOAT_CONVERT(U"1234.0", 1234.0, NULL);
})

TEST(convert_negative_float_token, {
	TEST_EVAL_FLOAT_CONVERT(U"-1234.0", -1234.0, NULL);
})

TEST(non_float_token_fails, {
	TEST_EVAL_FLOAT_CONVERT(U"not_a_float", 0, ERR_TYPE_MISMATCH);
})

#undef TEST_EVAL_FLOAT_CONVERT

void eval_float_test_self(bool *pass) {
	tests_t tests={
		test_convert_float_token,
		test_convert_negative_float_token,
		test_non_float_token_fails,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}