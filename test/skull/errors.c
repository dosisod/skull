#include <stdbool.h>

#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/eval/variable.h"

#include "test/skull/errors.h"
#include "test/testing.h"

TEST(is_error_msg, {
	ASSERT_TRUTHY(is_error_msg(ERR_TYPE_TYPE_BAD));
	ASSERT_FALSEY(is_error_msg(U"not an error msg"));
	ASSERT_FALSEY(is_error_msg(NULL));

	PASS;
})

bool fmt_error_wrapper(const char32_t *fmt, ErrorMsg msgs[], const char32_t *expected) {
	char32_t *out = fmt_error(fmt, msgs);
	if (!out || !expected) {
		return out == expected;
	}

	const bool pass = c32scmp(out, expected);

	free(out);
	return pass;
}

TEST(fmt_error, {
	ASSERT_TRUTHY(
		fmt_error_wrapper(U"%", (ErrorMsg[]){ {0} }, U"")
	);

	ASSERT_TRUTHY(
		fmt_error_wrapper(U"%", (ErrorMsg[]){ {0} }, U"")
	);

	ASSERT_TRUTHY(
		fmt_error_wrapper(U"%", (ErrorMsg[]){
			{ .str = U"abc" },
			{0}
		}, U"abc")
	);

	ASSERT_TRUTHY(
		fmt_error_wrapper(U"[%]", (ErrorMsg[]){
			{ .str = U"" },
			{0}
		}, U"[]")
	);

	ASSERT_TRUTHY(
		fmt_error_wrapper(U"[%]", (ErrorMsg[]){
			{ .str = U"abc" },
			{0}
		}, U"[abc]")
	);

	ASSERT_TRUTHY(
		fmt_error_wrapper(U"% %", (ErrorMsg[]){
			{ .str = U"hello" },
			{ .str = U"world" },
			{0}
		}, U"hello world")
	);

	ASSERT_TRUTHY(
		fmt_error_wrapper(U"%%", (ErrorMsg[]){
			{ .str = U"abc" },
			{ .str = U"def" },
			{0}
		}, U"abcdef")
	);

	ASSERT_TRUTHY(
		fmt_error_wrapper(U"testing_%", (ErrorMsg[]){
			{ .str = U"123" },
			{0}
		}, U"testing_123")
	);

	ASSERT_TRUTHY(
		fmt_error_wrapper(U"testing_%_testing", (ErrorMsg[]){
			{ .str = U"123" },
			{0}
		}, U"testing_123_testing")
	);

	PASS;
})

bool fmt_error_stringify_wrapper(ErrorMsg *error, const char32_t *expected) {
	fmt_error_stringify(error);

	if (!error->real) {
		return !expected;
	}
	const bool pass = c32scmp(error->real, expected);

	free(error->real);
	return pass;
}

TEST(fmt_error_stringify, {
	const Token *tok = tokenize(U"xxx");
	ErrorMsg err_tok = {
		.tok = tok
	};

	const Variable *var = make_variable("int", U"var_name", true);
	ErrorMsg err_var = {
		.var = var
	};

	const Variable *type_var = make_variable("int", U"var_name", true);
	ErrorMsg err_type = {
		.type = type_var->type
	};

	const char32_t *str = U"some string";
	ErrorMsg err_str = {
		.str = str
	};

	ASSERT_TRUTHY(fmt_error_stringify_wrapper(&err_tok, U"xxx"));
	ASSERT_TRUTHY(fmt_error_stringify_wrapper(&err_var, U"var_name"));
	ASSERT_TRUTHY(fmt_error_stringify_wrapper(&err_str, U"some string"));
	ASSERT_TRUTHY(fmt_error_stringify_wrapper(&err_type, U"int"));

	PASS;
})

TEST_SELF(error,
	test_is_error_msg,
	test_fmt_error,
	test_fmt_error_stringify
)
