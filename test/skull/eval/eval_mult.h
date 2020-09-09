#include <stdbool.h>

#include "skull/eval/eval_oper.h"
#include "skull/eval/types/defs.h"

#include "test/testing.h"

#define eval_mult(x, y) eval_oper((x)->type->multiply, x, y)

TEST(multiplying_2_ints, {
	Variable *var1=make_variable("int", U"var1", false);
	Variable *var2=make_variable("int", U"var2", false);

	const SkullInt num1=2;
	variable_write(var1, &num1);
	const SkullInt num2=3;
	variable_write(var2, &num2);
	Variable *var3=eval_mult(var1, var2);

	SkullInt result=0;
	variable_read(&result, var3);

	const bool pass=(result==6);

	free_variable(var1);
	free_variable(var2);
	free_variable(var3);

	return pass;
})

TEST(multiplying_2_floats, {
	Variable *var1=make_variable("float", U"var1", false);
	Variable *var2=make_variable("float", U"var2", false);

	const SkullFloat num1=2.0;
	variable_write(var1, &num1);
	const SkullFloat num2=3.0;
	variable_write(var2, &num2);
	Variable *var3=eval_mult(var1, var2);

	SkullFloat result=0.0;
	variable_read(&result, var3);

	//casting int to make compiler happy
	const bool pass=((int)result==(int)6.0);

	free_variable(var1);
	free_variable(var2);
	free_variable(var3);

	return pass;
})

TEST(multiplying_vars_with_different_types_fail, {
	Variable *var1=make_variable("int", U"var1", false);
	Variable *var2=make_variable("float", U"var2", false);
	Variable *var3=eval_mult(var1, var2);

	const bool pass = !var3;

	free_variable(var1);
	free_variable(var2);
	free(var3);

	return pass;
})

void eval_mult_test_self(bool *pass) {
	tests_t tests={
		test_multiplying_2_ints,
		test_multiplying_2_floats,
		test_multiplying_vars_with_different_types_fail,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
