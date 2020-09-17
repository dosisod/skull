#include <stdbool.h>

#include "skull/common/str.h"
#include "skull/eval/eval_oper.h"
#include "skull/eval/types/defs.h"

#include "test/skull/eval/eval_add.h"
#include "test/testing.h"

TEST(adding_2_ints, {
	Variable *var1 = make_variable("int", U"var1", false);
	Variable *var2 = make_variable("int", U"var2", false);

	const SkullInt num = 1;
	variable_write(var1, &num);
	variable_write(var2, &num);
	Variable *var3 = eval_add(var1, var2);

	SkullInt result = 0;
	variable_read(&result, var3);

	ASSERT_EQUAL(result, 2);

	free_variable(var1);
	free_variable(var2);
	free_variable(var3);

	PASS;
})

TEST(adding_2_floats, {
	Variable *var1 = make_variable("float", U"var1", false);
	Variable *var2 = make_variable("float", U"var2", false);

	const SkullFloat num = 1.0;
	variable_write(var1, &num);
	variable_write(var2, &num);
	Variable *var3 = eval_add(var1, var2);

	SkullFloat result = 0.0;
	variable_read(&result, var3);

	//casting to int to make compiler happy
	ASSERT_EQUAL((int)result, 2);

	free_variable(var1);
	free_variable(var2);
	free_variable(var3);

	PASS;
})

TEST(adding_2_strs, {
	Variable *var1 = make_variable("str", U"var1", false);
	Variable *var2 = make_variable("str", U"var2", false);

	const char32_t *str = U"x";
	variable_write(var1, &str);
	variable_write(var2, &str);
	Variable *var3 = eval_add(var1, var2);

	char32_t *result = NULL;
	variable_read(&result, var3);

	ASSERT_TRUTHY(c32scmp(result, U"xx"));

	free_variable(var1);
	free_variable(var2);
	free_variable(var3);

	PASS;
})

TEST(adding_vars_with_different_types_fail, {
	Variable *var1 = make_variable("int", U"var1", false);
	Variable *var2 = make_variable("float", U"var2", false);
	Variable *var3 = eval_add(var1, var2);

	ASSERT_FALSEY(var3);

	free_variable(var1);
	free_variable(var2);
	free(var3);

	PASS;
})

TEST_SELF(eval_add,
	test_adding_2_ints,
	test_adding_2_floats,
	test_adding_2_strs,
	test_adding_vars_with_different_types_fail
)
