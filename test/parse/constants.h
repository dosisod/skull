#include "../../src/parse/constants.h"
#include "../../test/testing.h"

TEST(find_line_comment_at_start_of_line, {
	const char32_t *code=U"# should be zero";

	return find_line_comment(code)==code;
})

TEST(find_line_comment_without_comment, {
	const char32_t *code=U"not a comment";

	return find_line_comment(code)==(code + 12);
})

TEST(find_line_comment_on_blank_line, {
	const char32_t *code=U"";

	return find_line_comment(code)==code;
})

void constants_test_self(bool *pass) {
	tests_t tests={
		test_find_line_comment_at_start_of_line,
		test_find_line_comment_without_comment,
		test_find_line_comment_on_blank_line,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}