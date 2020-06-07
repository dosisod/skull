#include <stdbool.h>

#include "../../src/eval/variable.h"
#include "../../test/testing.h"

bool test_create_variable(void) {
	make_default_types();
	variable_t *var=make_variable(L"int", L"x", true);

	const bool pass=(
		wcscmp(var->type->name, L"int")==0 &&
		wcscmp(var->name, L"x")==0 &&
		var->is_const &&
		var->bytes==8 &&
		var->mem != NULL
	);

	free_types();
	free_variable(var);

	return pass;
}

bool test_create_variable_with_invalid_type_fails(void) {
	const variable_t *var=make_variable(L"not_a_type", L"x", true);

	return var==NULL;
}

bool test_variable_write(void) {
	make_default_types();
	variable_t *var=make_variable(L"int", L"x", false);

	const int64_t data=1234;
	const uint8_t ret=variable_write(var, &data);

	int64_t val=0;
	memcpy(&val, var->mem, var->bytes);

	const bool pass=(
		ret==VARIABLE_WRITE_OK &&
		val==1234
	);

	make_default_types();
	free_variable(var);

	return pass;
}

bool test_variable_cannot_write_to_const(void) {
	make_default_types();
	variable_t *var=make_variable(L"int", L"x", true);

	const int64_t data=1234;
	const uint8_t ret=variable_write(var, &data);

	int64_t val=0;
	memcpy(&val, var->mem, var->bytes);

	const bool pass=(
		ret==VARIABLE_WRITE_ECONST &&
		val==0
	);

	free_types();
	free_variable(var);

	return pass;
}

bool test_variable_read(void) {
	make_default_types();
	variable_t *var=make_variable(L"int", L"x", false);
	const int64_t data=1234;
	variable_write(var, &data);

	int64_t val=0;
	variable_read(&val, var);

	const bool pass=(val==1234);

	free_types();
	free_variable(var);

	return pass;
}

bool test_make_variable_with_invalid_name_fails(void) {
	make_default_types();
	variable_t *var=make_variable(L"int", L"1nvalid", false);

	const bool pass=(var==NULL);

	free_types();
	free_variable(var);

	return pass;
}

bool test_free_variable(void) {
	make_default_types();
	variable_t *var=make_variable(L"int", L"x", true);

	if (var==NULL || var->mem==NULL) {
		free_variable(var);
		return false;
	}

	free_types();
	free_variable(var);

	return true;
}

bool test_free_null_variable_is_ok(void) {
	variable_t *var=NULL;

	free_variable(var);

	return true;
}

bool test_fmt_var_int(void) {
	make_default_types();
	variable_t *var=make_variable(L"int", L"x", false);

	int64_t data=1234;
	variable_write(var, &data);

	wchar_t *str=fmt_var(var);

	const bool pass=(
		str!=NULL &&
		wcscmp(L"1234", str)==0
	);

	free(str);
	free_variable(var);
	free_types();
	return pass;
}

bool test_fmt_var_float(void) {
	make_default_types();
	variable_t *var=make_variable(L"float", L"x", false);

	long double data=3.1415;
	variable_write(var, &data);

	wchar_t *str=fmt_var(var);

	const bool pass=(
		str!=NULL &&
		wcscmp(L"3.1415", str)==0
	);

	free(str);
	free_variable(var);
	free_types();
	return pass;
}

void variable_test_self(bool *pass) {
	tests_t tests={
		test_create_variable,
		test_create_variable_with_invalid_type_fails,
		test_variable_write,
		test_variable_cannot_write_to_const,
		test_variable_read,
		test_make_variable_with_invalid_name_fails,
		test_free_variable,
		test_free_null_variable_is_ok,
		test_fmt_var_int,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
