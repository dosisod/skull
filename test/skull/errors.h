#include <stdbool.h>

#include "skull/common/str.h"
#include "skull/errors.h"

#include "test/testing.h"

TEST(is_error_msg, {
	return (
		is_error_msg(ERR_INVALID_INPUT) &&
		is_error_msg(ERR_OVERFLOW) &&
		!is_error_msg(U"not an error msg") &&
		!is_error_msg(NULL)
	);
})

bool fmt_error_wrapper(const char32_t *fmt, const char32_t *strs[], const char32_t *expected) {
	char32_t *out = fmt_error(fmt, strs);
	if (!out) {
		return out == expected;
	}

	const bool pass = c32scmp(out, expected);

	free(out);
	return pass;
}

TEST(fmt_error, {
	return (
		fmt_error_wrapper(U"%", (const char32_t *[]){ NULL }, NULL) &&
		fmt_error_wrapper(U"%", (const char32_t *[]){ U"abc", NULL }, U"abc") &&
		fmt_error_wrapper(U"[%]", (const char32_t *[]){ U"", NULL }, U"[]") &&
		fmt_error_wrapper(U"[%]", (const char32_t *[]){ U"abc", NULL }, U"[abc]") &&
		fmt_error_wrapper(U"% %", (const char32_t *[]){ U"hello", U"world", NULL }, U"hello world") &&
		fmt_error_wrapper(U"%%", (const char32_t *[]){ U"abc", U"def", NULL }, U"abcdef")
	);
})

void error_test_self(bool *pass) {
	tests_t tests = {
		test_is_error_msg,
		test_fmt_error,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
