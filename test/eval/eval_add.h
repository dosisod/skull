#include <stdbool.h>

#include "../../src/eval/eval_add.h"
#include "../../test/testing.h"

TEST(adding_2_ints, {
	variable_t *var1=make_variable(U"int", U"var1", false);
	variable_t *var2=make_variable(U"int", U"var2", false);

	const int64_t num=1;
	variable_write(var1, &num);
	variable_write(var2, &num);
	variable_t *var3=eval_add(var1, var2);

	int64_t result=0;
	variable_read(&result, var3);

	const bool pass=(result==2);

	free_variable(var1);
	free_variable(var2);
	free_variable(var3);

	return pass;
})

TEST(adding_2_floats, {
	variable_t *var1=make_variable(U"float", U"var1", false);
	variable_t *var2=make_variable(U"float", U"var2", false);

	const long double num=1.0;
	variable_write(var1, &num);
	variable_write(var2, &num);
	variable_t *var3=eval_add(var1, var2);

	long double result=0.0;
	variable_read(&result, var3);

	//casting int to make compiler happy
	const bool pass=((int)result==(int)2.0);

	free_variable(var1);
	free_variable(var2);
	free_variable(var3);

	return pass;
})

TEST(adding_vars_with_different_types_fail, {
	variable_t *var1=make_variable(U"int", U"var1", false);
	variable_t *var2=make_variable(U"float", U"var2", false);
	variable_t *var3=eval_add(var1, var2);

	const bool pass=(var3==NULL);

	free_variable(var1);
	free_variable(var2);
	free(var3);

	return pass;
})

void eval_add_test_self(bool *pass) {
	tests_t tests={
		test_adding_2_ints,
		test_adding_2_floats,
		test_adding_vars_with_different_types_fail,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
