#include <stdbool.h>

#include "../src/eval/eval_if.h"
#include "../test/testing.h"

bool test_eval_if_single_param() {
	return (
		eval_if("true")==EVAL_IF_TRUE &&
		eval_if("false")==EVAL_IF_FALSE &&
		eval_if("not_a_bool")==EVAL_IF_ERROR
	);
}

bool test_eval_if_odd_num_tokens() {
	return eval_if("bad ==")==EVAL_IF_ERROR;
}

bool test_eval_if_equal_compare() {
	return (
		eval_if("true == true")==EVAL_IF_TRUE &&
		eval_if("true != true")==EVAL_IF_FALSE &&
		eval_if("false == true")==EVAL_IF_FALSE
	);
}

bool test_eval_if_with_errors() {
	return (
		eval_if("true == not_a_bool")==EVAL_IF_ERROR &&
		eval_if("not_a_bool == true")==EVAL_IF_ERROR
	);
}

bool test_eval_if_not() {
	return (
		eval_if("not true")==EVAL_IF_FALSE &&
		eval_if("not false")==EVAL_IF_TRUE
	);
}

void eval_if_test_self(bool *pass) {
	tests_t tests={
		test_eval_if_single_param,
		test_eval_if_odd_num_tokens,
		test_eval_if_equal_compare,
		test_eval_if_with_errors,
		test_eval_if_not,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
