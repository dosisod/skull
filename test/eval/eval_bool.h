#include <stdbool.h>

#include "../../src/eval/eval_bool.h"
#include "../../test/testing.h"

bool eval_bool_shim(const wchar_t *code, bool expected, const wchar_t *error) {
	const wchar_t *err=NULL;

	return (
		eval_bool(code, &err)==expected &&
		err==error
	);
}

TEST(eval_bool_single_param, {
	return (
		eval_bool_shim(L"true", true, NULL) &&
		eval_bool_shim(L"false", false, NULL) &&
		eval_bool_shim(L"not_a_bool", false, ERROR_MSG[ERROR_TYPE_MISMATCH])
	);
});

TEST(eval_bool_odd_num_tokens, {
	return eval_bool_shim(L"bad ==", false, ERROR_MSG[ERROR_TYPE_MISMATCH]);
});

TEST(eval_equality_comparison, {
	return (
		eval_bool_shim(L"true == true", true, NULL) &&
		eval_bool_shim(L"true != true", false, NULL) &&
		eval_bool_shim(L"false == true", false, NULL) &&
		eval_bool_shim(L"0.0 == 0", false, ERROR_MSG[ERROR_TYPE_MISMATCH]) &&
		eval_bool_shim(L"123 == 321", false, NULL) &&
		eval_bool_shim(L"3.14 == 3.14", true, NULL) &&
		eval_bool_shim(L"'x' == 'x'", true, NULL) &&
		eval_bool_shim(L"\"abc\" == \"abc\"", true, NULL)
	);
});

TEST(eval_bool_with_errors, {
	return (
		eval_bool_shim(L"true == 1nvalid", false, ERROR_MSG[ERROR_TYPE_MISMATCH]) &&
		eval_bool_shim(L"1nvalid == true", false, ERROR_MSG[ERROR_TYPE_MISMATCH]) &&
		eval_bool_shim(L"1nvalid == 1nvalid", false, ERROR_MSG[ERROR_TYPE_MISMATCH])
	);
});

TEST(eval_bool_not, {
	return (
		eval_bool_shim(L"not true", false, NULL) &&
		eval_bool_shim(L"not false", true, NULL)
	);
});

TEST(eval_bool_and, {
	return (
		eval_bool_shim(L"true and true", true, NULL) &&
		eval_bool_shim(L"false and false", false, NULL)
	);
});

TEST(eval_bool_or, {
	return (
		eval_bool_shim(L"true or false", true, NULL) &&
		eval_bool_shim(L"false or false", false, NULL)
	);
});

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
