#include <stdbool.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/common/errors.h"
#include "skull/semantic/variable.h"

#include "test/skull/semantic/variable.h"
#include "test/testing.h"

bool test_create_variable() {
	Variable *var = make_variable(TYPE_INT, U"x", true);

	ASSERT_EQUAL(strcmp(var->type, TYPE_INT), 0);
	ASSERT_EQUAL(strcmp(var->name, "x"), 0);
	ASSERT_TRUTHY(var->is_const);
	ASSERT_FALSEY(var->ref);

	free_variable(var);

	PASS
}

bool test_create_variable_with_invalid_type_fails() {
	ASSERT_FALSEY(make_variable(NULL, U"x", true));

	PASS
}

bool test_make_variable_with_invalid_name_fails() {
	Variable *var = make_variable(TYPE_INT, U"1nvalid", false);

	ASSERT_FALSEY(var);

	free_variable(var);

	PASS
}

bool test_free_variable() {
	free_variable(make_variable(TYPE_INT, U"x", true));

	PASS
}

bool test_free_null_variable_is_ok() {
	free_variable(NULL);

	PASS
}

void variable_test_self(bool *pass) {
	RUN_ALL(
		test_create_variable,
		test_create_variable_with_invalid_type_fails,
		test_make_variable_with_invalid_name_fails,
		test_free_variable,
		test_free_null_variable_is_ok
	)
}
