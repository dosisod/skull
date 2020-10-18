#include <stdbool.h>
#include <string.h>

#include "skull/common/str.h"
#include "skull/common/errors.h"
#include "skull/eval/types/defs.h"
#include "skull/eval/variable.h"
#include "skull/parse/classify.h"

#include "test/skull/eval/variable.h"
#include "test/testing.h"

TEST(create_variable, {
	Variable *var = make_variable(&TYPE_INT, U"x", true);

	ASSERT_EQUAL(strcmp(var->type->name, *&TYPE_INT.name), 0);
	ASSERT_TRUTHY(c32scmp(var->name, U"x"));
	ASSERT_TRUTHY(var->is_const);
	ASSERT_EQUAL(var->type->bytes, 8);
	ASSERT_FALSEY(var->mem);
	ASSERT_FALSEY(var->alloca);

	free_variable(var);
})

TEST(create_variable_with_invalid_type_fails, {
	ASSERT_FALSEY(make_variable(NULL, U"x", true));
})

TEST(variable_write, {
	Variable *var = make_variable(&TYPE_INT, U"x", false);

	const SkullInt data = 1234;
	char32_t *err = variable_write(var, &data);

	SkullInt val = 0;
	memcpy(&val, var->mem, var->type->bytes);

	ASSERT_FALSEY(err);
	ASSERT_EQUAL(val, 1234);

	free_variable(var);
})

TEST(variable_cannot_write_to_const, {
	Variable *var = make_variable(&TYPE_INT, U"x", true);

	// initial write
	SkullInt data = 1234;
	variable_write(var, &data);

	data = -1234;
	char32_t *err = variable_write(var, &data);

	SkullInt val = 0;
	memcpy(&val, var->mem, var->type->bytes);

	ASSERT_EQUAL(val, 1234);
	ASSERT_TRUTHY(c32scmp(
		ERR_CANNOT_ASSIGN_CONST_(U"x"),
		err
	));

	free(err);
	free_variable(var);
})

TEST(variable_read, {
	Variable *var = make_variable(&TYPE_INT, U"x", false);
	const SkullInt data = 1234;
	variable_write(var, &data);

	SkullInt val = 0;
	variable_read(&val, var);

	ASSERT_EQUAL(val, 1234);

	free_variable(var);
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
	test_variable_write,
	test_variable_cannot_write_to_const,
	test_variable_read,
	test_make_variable_with_invalid_name_fails,
	test_free_variable,
	test_free_null_variable_is_ok
)
