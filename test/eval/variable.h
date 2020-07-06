#include <stdbool.h>
#include <string.h>

#include "../../src/errors.h"
#include "../../src/eval/eval_assign.h"
#include "../../src/eval/variable.h"
#include "../../src/parse/classify.h"
#include "../../test/testing.h"

TEST(create_variable, {
	variable_t *var=make_variable(U"int", U"x", true);

	const bool pass=(
		c32scmp(var->type->name, U"int") &&
		c32scmp(var->name, U"x") &&
		var->is_const &&
		var->bytes==8 &&
		var->mem != NULL
	);

	free_variable(var);

	return pass;
})

TEST(create_variable_with_invalid_type_fails, {
	return (make_variable(U"not_a_type", U"x", true)==NULL);
})

TEST(variable_write, {
	variable_t *var=make_variable(U"int", U"x", false);

	const int64_t data=1234;
	const char32_t *ret=variable_write(var, &data);

	int64_t val=0;
	memcpy(&val, var->mem, var->bytes);

	const bool pass=(
		ret==NULL &&
		val==1234
	);

	free_variable(var);

	return pass;
})

TEST(variable_cannot_write_to_const, {
	variable_t *var=make_variable(U"int", U"x", true);

	const int64_t data=1234;
	const char32_t *ret=variable_write(var, &data);

	int64_t val=0;
	memcpy(&val, var->mem, var->bytes);

	const bool pass=(
		ret==ERR_CANNOT_ASSIGN_CONST &&
		val==0
	);

	free_variable(var);

	return pass;
})

TEST(variable_read, {
	variable_t *var=make_variable(U"int", U"x", false);
	const int64_t data=1234;
	variable_write(var, &data);

	int64_t val=0;
	variable_read(&val, var);

	const bool pass=(val==1234);

	free_variable(var);

	return pass;
})

TEST(make_variable_with_invalid_name_fails, {
	variable_t *var=make_variable(U"int", U"1nvalid", false);

	const bool pass=(var==NULL);

	free_variable(var);

	return pass;
})

TEST(free_variable, {
	variable_t *var=make_variable(U"int", U"x", true);

	if (var==NULL || var->mem==NULL) {
		free_variable(var);
		return false;
	}

	free_variable(var);

	return true;
})

TEST(free_null_variable_is_ok, {
	free_variable(NULL);

	return true;
})

#define TEST_FMT_VAR(str_type, real_type, real_data, expected) \
	variable_t *var=make_variable(str_type, U"x", false); \
	real_type data=real_data; \
	variable_write(var, &data); \
	char32_t *str=fmt_var(var); \
	const bool pass=( \
		str!=NULL && \
		c32scmp(expected, str) \
	); \
	free(str); \
	free_variable(var); \
	return pass;

TEST(fmt_var_int, {
	TEST_FMT_VAR(U"int", int64_t, 1234, U"1234");
})

TEST(fmt_var_float, {
	TEST_FMT_VAR(U"float", long double, 3.1415L, U"3.1415");
})

TEST(fmt_var_float_zeros, {
	TEST_FMT_VAR(U"float", long double, 1234, U"1234.0");
})

TEST(fmt_var_bool, {
	TEST_FMT_VAR(U"bool", bool, false, U"false");
})

TEST(fmt_var_char, {
	TEST_FMT_VAR(U"char", char32_t, U'a', U"a");
})

TEST(fmt_var_wide_char_preserved, {
	TEST_FMT_VAR(U"char", char32_t, U'存', U"存");
})

#undef TEST_FMT_VAR

TEST(fmt_var_str, {
	variable_t *var=make_variable(U"str", U"x", false);

	const char32_t *data=U"\"abc\"";
	token_t *token=tokenize(data);
	classify_token(token);

	eval_assign(var, token, NULL);

	char32_t *str=fmt_var(var);

	const bool pass=(
		str!=NULL &&
		c32scmp(U"abc", str)
	);

	char32_t *mem=NULL;
	variable_read(&mem, var);
	free(mem);

	free(str);
	free_variable(var);
	return pass;
})

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
		test_fmt_var_float,
		test_fmt_var_float_zeros,
		test_fmt_var_bool,
		test_fmt_var_char,
		test_fmt_var_wide_char_preserved,
		test_fmt_var_str,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
