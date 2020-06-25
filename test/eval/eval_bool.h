#include <stdbool.h>

#include "../../src/errors.h"
#include "../../src/eval/eval_bool.h"
#include "../../src/parse/classify.h"
#include "../../test/testing.h"

bool eval_bool_shim(const char32_t *code, bool expected, const char32_t *error) {
	const char32_t *err=NULL;

	token_t *token=tokenize(code);
	classify_tokens(token);

	const bool pass=(
		eval_bool(token, &err)==expected &&
		err==error
	);

	free_tokens(token);
	return pass;
}

TEST(eval_bool_single_param, {
	return (
		eval_bool_shim(U"true", true, NULL) &&
		eval_bool_shim(U"false", false, NULL) &&
		eval_bool_shim(U"not_a_bool", false, ERROR_MSG[ERROR_TYPE_MISMATCH])
	);
})

TEST(eval_bool_odd_num_tokens, {
	return eval_bool_shim(U"bad ==", false, ERROR_MSG[ERROR_TYPE_MISMATCH]);
})

TEST(eval_equality_comparison, {
	return (
		eval_bool_shim(U"true == true", true, NULL) &&
		eval_bool_shim(U"true != true", false, NULL) &&
		eval_bool_shim(U"false == true", false, NULL) &&
		eval_bool_shim(U"0.0 == 0", false, ERROR_MSG[ERROR_TYPE_MISMATCH]) &&
		eval_bool_shim(U"123 == 321", false, NULL) &&
		eval_bool_shim(U"3.14 == 3.14", true, NULL) &&
		eval_bool_shim(U"'x' == 'x'", true, NULL) &&
		eval_bool_shim(U"\"abc\" == \"abc\"", true, NULL)
	);
})

TEST(eval_bool_with_errors, {
	return (
		eval_bool_shim(U"true == 1nvalid", false, ERROR_MSG[ERROR_TYPE_MISMATCH]) &&
		eval_bool_shim(U"1nvalid == true", false, ERROR_MSG[ERROR_TYPE_MISMATCH]) &&
		eval_bool_shim(U"1nvalid == 1nvalid", false, ERROR_MSG[ERROR_TYPE_MISMATCH])
	);
})

TEST(eval_bool_not, {
	return (
		eval_bool_shim(U"not true", false, NULL) &&
		eval_bool_shim(U"not false", true, NULL)
	);
})

TEST(eval_bool_and, {
	return (
		eval_bool_shim(U"true and true", true, NULL) &&
		eval_bool_shim(U"false and false", false, NULL)
	);
})

TEST(eval_bool_or, {
	return (
		eval_bool_shim(U"true or false", true, NULL) &&
		eval_bool_shim(U"false or false", false, NULL)
	);
})

void eval_bool_test_self(bool *pass) {
	tests_t tests={
		test_eval_bool_single_param,
		test_eval_bool_odd_num_tokens,
		test_eval_equality_comparison,
		test_eval_bool_with_errors,
		test_eval_bool_not,
		test_eval_bool_and,
		test_eval_bool_or,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
