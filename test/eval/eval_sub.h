#include <stdbool.h>

#include "skull/common/str.h"
#include "skull/eval/eval_sub.h"

#include "test/testing.h"

TEST(subtracting_2_ints, {
	variable_t *var1=make_variable(U"int", U"var1", false);
	variable_t *var2=make_variable(U"int", U"var2", false);

	const int64_t num1=5;
	variable_write(var1, &num1);
	const int64_t num2=3;
	variable_write(var2, &num2);
	variable_t *var3=eval_sub(var1, var2);

	int64_t result=0;
	variable_read(&result, var3);

	const bool pass=(result==2);

	free_variable(var1);
	free_variable(var2);
	free_variable(var3);

	return pass;
})

TEST(subtracting_2_floats, {
	variable_t *var1=make_variable(U"float", U"var1", false);
	variable_t *var2=make_variable(U"float", U"var2", false);

	const long double num1=5.0L;
	variable_write(var1, &num1);
	const long double num2=3.0L;
	variable_write(var2, &num2);
	variable_t *var3=eval_sub(var1, var2);

	long double result=0.0L;
	variable_read(&result, var3);

	//casting int to make compiler happy
	const bool pass=((int)result==(int)2.0);

	free_variable(var1);
	free_variable(var2);
	free_variable(var3);

	return pass;
})

TEST(subtracting_vars_with_different_types_fail, {
	variable_t *var1=make_variable(U"int", U"var1", false);
	variable_t *var2=make_variable(U"float", U"var2", false);
	variable_t *var3=eval_sub(var1, var2);

	const bool pass=(var3==NULL);

	free_variable(var1);
	free_variable(var2);
	free(var3);

	return pass;
})

void eval_sub_test_self(bool *pass) {
	tests_t tests={
		test_subtracting_2_ints,
		test_subtracting_2_floats,
		test_subtracting_vars_with_different_types_fail,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
