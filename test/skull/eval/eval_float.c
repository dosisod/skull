#include <math.h>
#include <stdbool.h>

#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/eval/types/defs.h"
#include "skull/eval/types/float.h"
#include "skull/parse/classify.h"

#include "test/skull/eval/eval_float.h"
#include "test/testing.h"

TEST(convert_float_token, {
	TEST_EVAL_FLOAT_CONVERT(U"1234.0", (int)num == 1234, NULL);
})

TEST(convert_negative_float_token, {
	TEST_EVAL_FLOAT_CONVERT(U"-1234.0", (int)num == -1234, NULL);
})

#define TEMP_FLOAT U"999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0"
TEST(convert_overflow_float, {
	TEST_EVAL_FLOAT_CONVERT(
		TEMP_FLOAT,
		isinf(num),
		ERR_OVERFLOW_(TEMP_FLOAT)
	);
})
#undef TEMP_FLOAT

TEST(convert_infinity_float_token, {
	TEST_EVAL_FLOAT_CONVERT(U"Infinity", isinf(num), NULL);
})

TEST(convert_neg_infinity_float_token, {
	TEST_EVAL_FLOAT_CONVERT(U"-Infinity", isinf(num), NULL);
})

#undef TEST_EVAL_FLOAT_CONVERT

TEST_SELF(eval_float,
	test_convert_float_token,
	test_convert_negative_float_token,
	test_convert_infinity_float_token,
	test_convert_overflow_float,
	test_convert_neg_infinity_float_token
)