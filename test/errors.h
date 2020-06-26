#include <stdbool.h>

#include "../src/errors.h"

#include "../test/testing.h"

TEST(errors_not_null, {
	return (
		ERROR_MSG[ERROR_INVALID_INPUT]!=NULL &&
		ERROR_MSG[ERROR_VAR_ALREADY_DEFINED]!=NULL &&
		ERROR_MSG[ERROR_CANNOT_ASSIGN_CONST]!=NULL &&
		ERROR_MSG[ERROR_TYPE_MISMATCH]!=NULL &&
		ERROR_MSG[ERROR_OVERFLOW]!=NULL &&
		ERROR_MSG[ERROR_MISSING_ASSIGNMENT]!=NULL &&
		ERROR_MSG[ERROR_VAR_NOT_FOUND]!=NULL
	);
})

TEST(is_error_msg_setup, {
	ERROR_MSG_LAST=NULL;

	is_error_msg(U"anything");

	return (
		*ERROR_MSG_LAST!=NULL &&
		*(ERROR_MSG_LAST + 1)[0]==U'\0'
	);
})

TEST(is_error_msg, {
	return (
		is_error_msg(ERROR_MSG[ERROR_INVALID_INPUT]) &&
		is_error_msg(ERROR_MSG[ERROR_OVERFLOW]) &&
		!is_error_msg(U"not an error msg")
	);
})

TEST(is_error_msg_with_null_returns_false, {
	return !is_error_msg(NULL);
})

void error_test_self(bool *pass) {
	tests_t tests={
		test_errors_not_null,
		test_is_error_msg_setup,
		test_is_error_msg,
		test_is_error_msg_with_null_returns_false,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
