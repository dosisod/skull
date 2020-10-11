#include <limits.h>
#include <stdbool.h>

#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/eval/types/defs.h"
#include "skull/eval/types/int.h"
#include "skull/parse/classify.h"

#include "test/skull/eval/eval_integer.h"
#include "test/testing.h"

TEST(convert_integer_token, {
	TEST_EVAL_INT_CONVERT(U"1234", 1234, NULL);
})

TEST(convert_negative_integer_token, {
	TEST_EVAL_INT_CONVERT(U"-1234", -1234, NULL);
})

#define TEMP_INT U"9999999999999999999"
TEST(integer_overflow_returns_error, {
	TEST_EVAL_INT_CONVERT(TEMP_INT, LLONG_MAX, ERR_OVERFLOW_(TEMP_INT));
})

TEST(integer_underflow_returns_error, {
	TEST_EVAL_INT_CONVERT(U"-" TEMP_INT, LLONG_MIN, ERR_OVERFLOW_(U"-"TEMP_INT));
})
#undef TEMP_INT

TEST(convert_hex_integer, {
	TEST_EVAL_INT_CONVERT(U"0xff", 255, NULL);
})

TEST(convert_octal_integer, {
	TEST_EVAL_INT_CONVERT(U"0o777", 0777, NULL);
})

TEST(convert_binary_integer, {
	TEST_EVAL_INT_CONVERT(U"0b1111", 15, NULL);
})

#undef TEST_EVAL_INT_CONVERT

TEST_SELF(eval_integer,
	test_convert_integer_token,
	test_convert_negative_integer_token,
	test_integer_overflow_returns_error,
	test_integer_underflow_returns_error,
	test_convert_hex_integer,
	test_convert_octal_integer,
	test_convert_binary_integer
)