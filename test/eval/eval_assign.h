#include <stdbool.h>

#include "../../src/eval/eval_assign.h"
#include "../../test/testing.h"

bool test_eval_assign_int(void) {
	token_t *token=tokenize(L"1234");
	classify_tokens(token);

	uint8_t err=0;
	int64_t *data=eval_assign(token, &err);

	const bool pass=(*data==1234);

	free(data);
	return pass;
}

bool test_eval_assign_float(void) {
	token_t *token=tokenize(L"0.0");
	classify_tokens(token);

	uint8_t err=0;
	long double *data=eval_assign(token, &err);

	const bool pass=(*data==0.0);

	free(data);
	return pass;
}

bool test_eval_assign_bool(void) {
	token_t *token=tokenize(L"false");
	classify_tokens(token);

	uint8_t err=0;
	bool *data=eval_assign(token, &err);

	const bool pass=(*data==false);

	free(data);
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
