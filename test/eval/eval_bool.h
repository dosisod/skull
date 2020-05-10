#include <stdbool.h>

#include "../src/eval/eval_bool.h"
#include "../test/testing.h"

bool test_eval_bool_single_param() {
	return (
		eval_bool(L"true")==EVAL_TRUE &&
		eval_bool(L"false")==EVAL_FALSE &&
		eval_bool(L"not_a_bool")==EVAL_ERROR
	);
}

bool test_eval_bool_odd_num_tokens() {
	return eval_bool(L"bad ==")==EVAL_ERROR;
}

bool test_eval_equality_comparison() {
	return (
		eval_bool(L"true == true")==EVAL_TRUE &&
		eval_bool(L"true != true")==EVAL_FALSE &&
		eval_bool(L"false == true")==EVAL_FALSE &&
		eval_bool(L"0.0 == 0")==EVAL_ERROR &&
		eval_bool(L"123 == 321")==EVAL_FALSE &&
		eval_bool(L"3.14 == 3.14")==EVAL_TRUE &&
		eval_bool(L"'x' == 'x'")==EVAL_TRUE &&
		eval_bool(L"\"abc\" == \"abc\"")==EVAL_TRUE
	);
}

bool test_eval_bool_with_errors() {
	return (
		eval_bool(L"true == not_a_bool")==EVAL_ERROR &&
		eval_bool(L"not_a_bool == true")==EVAL_ERROR &&
		eval_bool(L"not_a_bool == not_a_bool")==EVAL_ERROR
	);
}

bool test_eval_bool_not() {
	return (
		eval_bool(L"not true")==EVAL_FALSE &&
		eval_bool(L"not false")==EVAL_TRUE
	);
}

bool test_eval_bool_and() {
	return (
		eval_bool(L"true and true")==EVAL_TRUE &&
		eval_bool(L"false and false")==EVAL_FALSE
	);
}

bool test_eval_bool_or() {
	return (
		eval_bool(L"true or false")==EVAL_TRUE &&
		eval_bool(L"false or false")==EVAL_FALSE
	);
}

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
