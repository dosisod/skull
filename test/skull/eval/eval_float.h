#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include "skull/errors.h"
#include "skull/eval/eval_float.h"
#include "skull/parse/classify.h"

#include "test/testing.h"

#define TEST_EVAL_FLOAT_CONVERT(str_value, cond, expected_error) \
	token_t *token=tokenize(str_value); \
	classify_tokens(token); \
	const char32_t *err=NULL; \
	double *num=eval_float(token, &err); \
	const bool pass=( \
		(cond) && \
		(err==(expected_error) || c32scmp(expected_error, err)) \
	); \
	free(token); \
	free(num); \
	return pass;

TEST(convert_float_token, {
	TEST_EVAL_FLOAT_CONVERT(U"1234.0", (int)*num==1234, NULL);
})

TEST(convert_negative_float_token, {
	TEST_EVAL_FLOAT_CONVERT(U"-1234.0", (int)*num==-1234, NULL);
})

#define TEMP_FLOAT U"999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0"
TEST(convert_overflow_float, {
	TEST_EVAL_FLOAT_CONVERT(
		TEMP_FLOAT,
		isinf(*num),
		_ERR_OVERFLOW(TEMP_FLOAT)
	);
})
#undef TEMP_FLOAT

TEST(convert_infinity_float_token, {
	TEST_EVAL_FLOAT_CONVERT(U"Infinity", isinf(*num), NULL);
})

TEST(convert_neg_infinity_float_token, {
	TEST_EVAL_FLOAT_CONVERT(U"-Infinity", isinf(*num), NULL);
})

TEST(non_float_token_fails, {
	TEST_EVAL_FLOAT_CONVERT(U"not_a_float", true, _ERR_TYPE_MISMATCH(U"float"));
})

#undef TEST_EVAL_FLOAT_CONVERT

void eval_float_test_self(bool *pass) {
	tests_t tests={
		test_convert_float_token,
		test_convert_negative_float_token,
		test_convert_infinity_float_token,
		test_convert_overflow_float,
		test_convert_neg_infinity_float_token,
		test_non_float_token_fails,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}