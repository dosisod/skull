#include <stdbool.h>

#include "skull/eval/eval_div.h"

#include "test/testing.h"

TEST(dividing_2_ints, {
	variable_t *var1=make_variable("int", U"var1", false);
	variable_t *var2=make_variable("int", U"var2", false);

	const int64_t num1=6;
	variable_write(var1, &num1);
	const int64_t num2=3;
	variable_write(var2, &num2);
	variable_t *var3=eval_div(var1, var2);

	int64_t result=0;
	variable_read(&result, var3);

	const bool pass=(result==2);

	free_variable(var1);
	free_variable(var2);
	free_variable(var3);

	return pass;
})

TEST(divide_by_zero, {
	variable_t *var=make_variable("int", U"var", false);

	const int64_t num=0;
	variable_write(var, &num);

	const bool pass = !eval_div(var, var);

	free_variable(var);

	return pass;
})

TEST(dividing_2_floats, {
	variable_t *var1=make_variable("float", U"var1", false);
	variable_t *var2=make_variable("float", U"var2", false);

	const double num1=6.0;
	variable_write(var1, &num1);
	const double num2=3.0;
	variable_write(var2, &num2);
	variable_t *var3=eval_div(var1, var2);

	double result=0.0;
	variable_read(&result, var3);

	//casting int to make compiler happy
	const bool pass=((int)result==(int)2.0);

	free_variable(var1);
	free_variable(var2);
	free_variable(var3);

	return pass;
})

TEST(dividing_vars_with_different_types_fail, {
	variable_t *var1=make_variable("int", U"var1", false);
	variable_t *var2=make_variable("float", U"var2", false);
	variable_t *var3=eval_div(var1, var2);

	const bool pass = !var3;

	free_variable(var1);
	free_variable(var2);
	free(var3);

	return pass;
})

void eval_div_test_self(bool *pass) {
	tests_t tests={
		test_dividing_2_ints,
		test_divide_by_zero,
		test_dividing_2_floats,
		test_dividing_vars_with_different_types_fail,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
