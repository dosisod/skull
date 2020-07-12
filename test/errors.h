#include <stdbool.h>

#include "../src/errors.h"
#include "testing.h"

TEST(is_error_msg, {
	return (
		is_error_msg(ERR_INVALID_INPUT) &&
		is_error_msg(ERR_OVERFLOW) &&
		!is_error_msg(U"not an error msg") &&
		!is_error_msg(NULL)
	);
})

void error_test_self(bool *pass) {
	tests_t tests={
		test_is_error_msg,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
