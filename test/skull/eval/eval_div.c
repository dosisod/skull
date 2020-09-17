#include <stdbool.h>

#include "skull/eval/eval_oper.h"
#include "skull/eval/types/defs.h"

#include "test/skull/eval/eval_div.h"
#include "test/testing.h"

TEST(dividing_2_ints, {
	Variable *var1 = make_variable("int", U"var1", false);
	Variable *var2 = make_variable("int", U"var2", false);

	const SkullInt num1 = 6;
	variable_write(var1, &num1);
	const SkullInt num2 = 3;
	variable_write(var2, &num2);
	Variable *var3 = eval_div(var1, var2);

	SkullInt result = 0;
	variable_read(&result, var3);

	ASSERT_EQUAL(result, 2);

	free_variable(var1);
	free_variable(var2);
	free_variable(var3);

	PASS;
})

TEST(divide_by_zero, {
	Variable *var = make_variable("int", U"var", false);

	const SkullInt num = 0;
	variable_write(var, &num);

	ASSERT_FALSEY(eval_div(var, var));

	free_variable(var);
	PASS;
})

TEST(dividing_2_floats, {
	Variable *var1 = make_variable("float", U"var1", false);
	Variable *var2 = make_variable("float", U"var2", false);

	const SkullFloat num1 = 6.0;
	variable_write(var1, &num1);
	const SkullFloat num2 = 3.0;
	variable_write(var2, &num2);
	Variable *var3 = eval_div(var1, var2);

	SkullFloat result = 0.0;
	variable_read(&result, var3);

	ASSERT_EQUAL((int)result, 2);

	free_variable(var1);
	free_variable(var2);
	free_variable(var3);

	PASS;
})

TEST(dividing_vars_with_different_types_fail, {
	Variable *var1 = make_variable("int", U"var1", false);
	Variable *var2 = make_variable("float", U"var2", false);
	Variable *var3 = eval_div(var1, var2);

	ASSERT_FALSEY(var3);

	free_variable(var1);
	free_variable(var2);
	free(var3);

	PASS;
})

TEST_SELF(eval_div,
	test_dividing_2_ints,
	test_divide_by_zero,
	test_dividing_2_floats,
	test_dividing_vars_with_different_types_fail
)
