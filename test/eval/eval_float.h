#include <stdbool.h>
#include <stdint.h>

#include "../../src/eval/eval_float.h"
#include "../../test/testing.h"

TEST(convert_float_token, {
	token_t *token1=tokenize(L"1234.0");
	token_t *token2=tokenize(L"-1234.0");
	classify_tokens(token1);
	classify_tokens(token2);

	const wchar_t *err1=0;
	long double num1=eval_float(token1, &err1);
	const wchar_t *err2=0;
	long double num2=eval_float(token2, &err2);

	const bool pass=(
		num1==1234.0 &&
		err1==NULL &&
		num2==-1234.0 &&
		err2==NULL
	);

	free(token1);
	free(token2);

	return pass;
});

TEST(non_float_token_fails, {
	token_t *token=tokenize(L"not_an_int_token");
	classify_tokens(token);

	const wchar_t *err=NULL;
	long double num=eval_float(token, &err);

	const bool pass=(
		num==0 &&
		err==ERROR_MSG[ERROR_TYPE_MISMATCH]
	);

	free(token);

	return pass;
});

void eval_float_test_self(bool *pass) {
	tests_t tests={
		test_convert_float_token,
		test_non_float_token_fails,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}