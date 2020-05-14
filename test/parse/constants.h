#include "../../src/parse/constants.h"
#include "../../test/testing.h"

bool test_find_line_comment_at_start_of_line() {
	return find_line_comment("# should be zero")==0;
}

bool test_find_line_comment_without_comment() {
	return find_line_comment("not a comment")==12;
}

bool test_find_line_comment_on_blank_line() {
	return find_line_comment("")==0;
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