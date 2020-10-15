#include <stdbool.h>

#include "skull/common/str.h"
#include "skull/common/errors.h"
#include "skull/eval/variable.h"

#include "test/skull/common/errors.h"
#include "test/testing.h"

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
		fmt_error_wrapper(U"error here", (ErrorMsg[]){ {0} }, U"error here")
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
	Token *tok = tokenize(U"xxx");
	ErrorMsg err_tok = {
		.tok = tok
	};

	Variable *var = make_variable(&TYPE_INT, U"var_name", true);
	ErrorMsg err_var = {
		.var = var
	};

	ErrorMsg err_type = {
		.type = &TYPE_INT
	};

	const char32_t *str = U"some string";
	ErrorMsg err_str = {
		.str = str
	};

	ASSERT_TRUTHY(fmt_error_stringify_wrapper(&err_tok, U"xxx"));
	ASSERT_TRUTHY(fmt_error_stringify_wrapper(&err_var, U"var_name"));
	ASSERT_TRUTHY(fmt_error_stringify_wrapper(&err_str, U"some string"));
	ASSERT_TRUTHY(fmt_error_stringify_wrapper(&err_type, U"int"));

	free_tokens(tok);
	free_variable(var);
})

TEST_SELF(error,
	test_fmt_error,
	test_fmt_error_stringify
)
