#include <stdbool.h>

#include "../../src/eval/variable.h"
#include "../../test/testing.h"

bool test_create_variable() {
	variable_t *var=make_variable(L"int", L"x", true);

	const bool pass=(
		wcscmp(var->type->name, L"int")==0 &&
		wcscmp(var->name, L"x")==0 &&
		var->is_const &&
		var->bytes==8 &&
		var->mem != NULL
	);

	free_variable_t(var);

	return pass;
}

bool test_create_variable_with_invalid_type_fails() {
	const variable_t *var=make_variable(L"not_a_type", L"x", true);

	return var==NULL;
}

bool test_variable_write() {
	variable_t *var=make_variable(L"int", L"x", false);

	const int64_t data=1234;
	const uint8_t ret=variable_write(var, &data);

	int64_t val=0;
	memcpy(&val, var->mem, var->bytes);

	const bool pass=(
		ret==VARIABLE_WRITE_OK &&
		val==1234
	);

	free_variable_t(var);

	return pass;
}

bool test_variable_cannot_write_to_const() {
	variable_t *var=make_variable(L"int", L"x", true);

	const int64_t data=1234;
	const uint8_t ret=variable_write(var, &data);

	int64_t val=0;
	memcpy(&val, var->mem, var->bytes);

	const bool pass=(
		ret==VARIABLE_WRITE_ECONST &&
		val==0
	);

	free_variable_t(var);

	return pass;
}

bool test_variable_read() {
	variable_t *var=make_variable(L"int", L"x", false);
	const int64_t data=1234;
	variable_write(var, &data);

	int64_t val=0;
	variable_read(&val, var);

	const bool pass=(val==1234);

	free_variable_t(var);

	return pass;
}

bool test_free_variable_t() {
	variable_t *var=make_variable(L"int", L"x", true);

	if (var==NULL || var->mem==NULL) {
		free_variable_t(var);
		return false;
	}

	free_variable_t(var);

	return true;
}

void variable_test_self(bool *pass) {
	tests_t tests={
		test_create_variable,
		test_create_variable_with_invalid_type_fails,
		test_variable_write,
		test_variable_cannot_write_to_const,
		test_variable_read,
		test_free_variable_t,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
