#include <stdbool.h>
#include <stdint.h>

#include "../../src/eval/eval_integer.h"
#include "../../test/testing.h"

TEST(convert_integer_token, {
	token_t *token1=tokenize(L"1234");
	token_t *token2=tokenize(L"-1234");
	classify_tokens(token1);
	classify_tokens(token2);

	const wchar_t *err1=0;
	int64_t num1=eval_integer(token1, &err1);
	const wchar_t *err2=0;
	int64_t num2=eval_integer(token2, &err2);

	const bool pass=(
		num1==1234 &&
		err1==NULL &&
		num2==-1234 &&
		err2==NULL
	);

	free(token1);
	free(token2);

	return pass;
});

TEST(integer_overflow_returns_error, {
	token_t *token=tokenize(L"9999999999999999999");
	classify_tokens(token);

	const wchar_t *err=0;
	int64_t num=eval_integer(token, &err);

	const bool pass=(
		num==LLONG_MAX &&
		err==ERROR_MSG[ERROR_OVERFLOW]
	);

	free(token);

	return pass;
});

TEST(integer_underflow_returns_error, {
	token_t *token=tokenize(L"-9999999999999999999");
	classify_tokens(token);

	const wchar_t *err=0;
	int64_t num=eval_integer(token, &err);

	const bool pass=(
		num==LLONG_MIN &&
		err==ERROR_MSG[ERROR_OVERFLOW]
	);

	free(token);

	return pass;
});

TEST(convert_hex_integer, {
	token_t *token=tokenize(L"0xff");
	classify_tokens(token);

	const wchar_t *err=0;
	int64_t num=eval_integer(token, &err);

	const bool pass=(
		num==255 &&
		err==NULL
	);

	free(token);

	return pass;
});

TEST(convert_octal_integer, {
	token_t *token=tokenize(L"0o777");
	classify_tokens(token);

	const wchar_t *err=0;
	int64_t num=eval_integer(token, &err);

	const bool pass=(
		num==0777 &&
		err==NULL
	);

	free(token);

	return pass;
});

TEST(convert_binary_integer, {
	token_t *token=tokenize(L"0b1111");
	classify_tokens(token);

	const wchar_t *err=0;
	int64_t num=eval_integer(token, &err);

	const bool pass=(
		num==15 &&
		err==NULL
	);

	free(token);

	return pass;
});

TEST(non_integer_token_fails, {
	token_t *token=tokenize(L"not_an_int_token");
	classify_tokens(token);

	const wchar_t *err=0;
	int64_t num=eval_integer(token, &err);

	const bool pass=(
		num==0 &&
		err==ERROR_MSG[ERROR_TYPE_MISMATCH]
	);

	free(token);

	return pass;
});

void eval_integer_test_self(bool *pass) {
	tests_t tests={
		test_convert_integer_token,
		test_integer_overflow_returns_error,
		test_integer_underflow_returns_error,
		test_convert_hex_integer,
		test_convert_octal_integer,
		test_convert_binary_integer,
		test_non_integer_token_fails,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}