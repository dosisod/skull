#include <stdbool.h>

#include "../src/eval/variable.h"
#include "../test/testing.h"

bool test_create_variable() {
	variable_t *var=make_variable("i32", "x", 4);

	bool pass=(
		strcmp(var->type, "i32")==0 &&
		strcmp(var->name, "x")==0 &&
		var->bytes == 4 &&
		var->mem != NULL
	);

	free_variable_t(var);

	return pass;
}

bool test_create_variable_with_invalid_type_fails() {
	variable_t *var=make_variable("not_a_type", "x", 1);

	return var==NULL;
}

bool test_free_variable_t() {
	variable_t *var=make_variable("i32", "x", 4);

	if (var==NULL || var->mem==NULL) return false;

	free_variable_t(var);

	return (var!=NULL && var->mem!=NULL);
}

void variable_test_self(bool *pass) {
	tests_t tests={
		test_create_variable,
		test_create_variable_with_invalid_type_fails,
		test_free_variable_t,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
