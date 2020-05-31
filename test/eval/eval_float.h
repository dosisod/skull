#include <stdbool.h>
#include <stdint.h>

#include "../../src/eval/eval_float.h"
#include "../../test/testing.h"

bool test_convert_float_token(void) {
	token_t *token1=tokenize(L"1234.0");
	token_t *token2=tokenize(L"-1234.0");
	classify_tokens(token1);
	classify_tokens(token2);

	uint8_t err1=0;
	long double num1=eval_float(token1, &err1);
	uint8_t err2=0;
	long double num2=eval_float(token2, &err2);

	const bool pass=(
		num1==1234.0 &&
		err1==EVAL_FLOAT_OK &&
		num2==-1234.0 &&
		err2==EVAL_FLOAT_OK
	);

	free(token1);
	free(token2);

	return pass;
}

bool test_non_float_token_fails(void) {
	token_t *token=tokenize(L"not_an_int_token");
	classify_tokens(token);

	uint8_t err=0.0;
	long double num=eval_float(token, &err);

	const bool pass=(
		num==0 &&
		err==EVAL_FLOAT_ERR
	);

	free(token);

	return pass;
}

void eval_float_test_self(bool *pass) {
	tests_t tests={
		test_convert_float_token,
		test_non_float_token_fails,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}