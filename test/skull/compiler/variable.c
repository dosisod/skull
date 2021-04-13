#include <stdbool.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/common/errors.h"
#include "skull/compiler/types/defs.h"
#include "skull/compiler/variable.h"

#include "test/skull/compiler/variable.h"
#include "test/testing.h"

TEST(create_variable, {
	Variable *var = make_variable(&TYPE_INT, U"x", true);

	ASSERT_EQUAL(strcmp(var->type->name, *&TYPE_INT.name), 0);
	ASSERT_EQUAL(strcmp(var->name, "x"), 0);
	ASSERT_TRUTHY(var->is_const);
	ASSERT_FALSEY(var->value);

	free_variable(var);
})

TEST(create_variable_with_invalid_type_fails, {
	ASSERT_FALSEY(make_variable(NULL, U"x", true));
})

TEST(make_variable_with_invalid_name_fails, {
	Variable *var = make_variable(&TYPE_INT, U"1nvalid", false);

	ASSERT_FALSEY(var);

	free_variable(var);
})

TEST(free_variable, {
	free_variable(make_variable(&TYPE_INT, U"x", true));
})

TEST(free_null_variable_is_ok, {
	free_variable(NULL);
})

TEST_SELF(variable,
	test_create_variable,
	test_create_variable_with_invalid_type_fails,
	test_make_variable_with_invalid_name_fails,
	test_free_variable,
	test_free_null_variable_is_ok
)
