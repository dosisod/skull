#include <stdbool.h>

#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/eval/variable.h"

#include "test/testing.h"

TEST(is_error_msg, {
	return (
		is_error_msg(ERR_INVALID_INPUT) &&
		is_error_msg(ERR_OVERFLOW) &&
		!is_error_msg(U"not an error msg") &&
		!is_error_msg(NULL)
	);
})

bool fmt_error_wrapper(const char32_t *fmt, error_msg_t msgs[], const char32_t *expected) {
	char32_t *out = fmt_error(fmt, msgs);
	if (!out || !expected) {
		return out == expected;
	}

	const bool pass = c32scmp(out, expected);

	free(out);
	return pass;
}

TEST(fmt_error, {
	return (
		fmt_error_wrapper(U"%", (error_msg_t[]){ NULL }, U"") &&

		fmt_error_wrapper(U"%", (error_msg_t[]){
			{ .str = U"abc" },
			{0}
		}, U"abc") &&

		fmt_error_wrapper(U"[%]", (error_msg_t[]){
			{ .str = U"" },
			{0}
		}, U"[]") &&

		fmt_error_wrapper(U"[%]", (error_msg_t[]){
			{ .str = U"abc" },
			{0}
		}, U"[abc]") &&

		fmt_error_wrapper(U"% %", (error_msg_t[]){
			{ .str = U"hello" },
			{ .str = U"world" },
			{0}
		}, U"hello world") &&

		fmt_error_wrapper(U"%%", (error_msg_t[]){
			{ .str = U"abc" },
			{ .str = U"def" },
			{0}
		}, U"abcdef") &&

		fmt_error_wrapper(U"testing_%", (error_msg_t[]){
			{ .str = U"123" },
			{0}
		}, U"testing_123") &&

		fmt_error_wrapper(U"testing_%_testing", (error_msg_t[]){
			{ .str = U"123" },
			{0}
		}, U"testing_123_testing")
	);
})

bool fmt_error_stringify_wrapper(error_msg_t *error, const char32_t *expected) {
	fmt_error_stringify(error);

	if (!error->real) {
		return !expected;
	}
	const bool pass = c32scmp(error->real, expected);

	free(error->real);
	return pass;
}

TEST(fmt_error_stringify, {
	const token_t *tok = tokenize(U"xxx");
	error_msg_t err_tok = {
		.tok = tok
	};

	const variable_t *var = make_variable(U"int", U"var_name", true);
	error_msg_t err_var = {
		.var = var
	};

	const char32_t *str = U"some string";
	error_msg_t err_str = {
		.str = str
	};

	return (
		fmt_error_stringify_wrapper(&err_tok, U"xxx") &&
		fmt_error_stringify_wrapper(&err_var, U"var_name") &&
		fmt_error_stringify_wrapper(&err_str, U"some string")
	);
})

void error_test_self(bool *pass) {
	tests_t tests = {
		test_is_error_msg,
		test_fmt_error,
		test_fmt_error_stringify,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
