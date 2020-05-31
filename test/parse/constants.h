#include "../../src/parse/constants.h"
#include "../../test/testing.h"

bool test_find_line_comment_at_start_of_line(void) {
	const wchar_t *code=L"# should be zero";

	return find_line_comment(code)==code;
}

bool test_find_line_comment_without_comment(void) {
	const wchar_t *code=L"not a comment";

	return find_line_comment(code)==(code + 12);
}

bool test_find_line_comment_on_blank_line(void) {
	const wchar_t *code=L"";

	return find_line_comment(code)==code;
}

void constants_test_self(bool *pass) {
	tests_t tests={
		test_find_line_comment_at_start_of_line,
		test_find_line_comment_without_comment,
		test_find_line_comment_on_blank_line,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}