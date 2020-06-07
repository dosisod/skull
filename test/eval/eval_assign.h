#include <stdbool.h>

#include "../../src/eval/eval_assign.h"
#include "../../test/testing.h"

bool test_eval_assign_int(void) {
	token_t *token=tokenize(L"1234");
	classify_tokens(token);

	make_default_types();
	variable_t *var=make_variable(L"int", L"x", false);

	eval_assign(var, token);

	int64_t data=0;
	variable_read(&data, var);

	const bool pass=(data==1234);

	free_types();
	free_variable(var);
	return pass;
}

bool test_eval_assign_float(void) {
	token_t *token=tokenize(L"0.0");
	classify_tokens(token);

	make_default_types();
	variable_t *var=make_variable(L"float", L"x", false);

	eval_assign(var, token);

	long double data=1.0;
	variable_read(&data, var);

	const bool pass=(data==0.0);

	free_types();
	free_variable(var);
	return pass;
}

bool test_eval_assign_bool(void) {
	token_t *token=tokenize(L"false");
	classify_tokens(token);

	make_default_types();
	variable_t *var=make_variable(L"int", L"x", false);

	eval_assign(var, token);

	bool data=true;
	variable_read(&data, var);

	const bool pass=(data==false);

	free_types();
	free_variable(var);
	return pass;
}

void eval_assign_test_self(bool *pass) {
	tests_t tests={
		test_eval_assign_int,
		test_eval_assign_float,
		test_eval_assign_bool,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
