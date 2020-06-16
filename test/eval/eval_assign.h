#include <stdbool.h>

#include "../../src/eval/eval_assign.h"
#include "../../test/testing.h"

#define TEST_EVAL_ASSIGN_BASE(str_type, str_value, real_type, expected_val, expected_error, cmp) \
	token_t *token=tokenize(str_value); \
	classify_tokens(token); \
	make_default_types(); \
	variable_t *var=make_variable(str_type, L"x", false); \
	const wchar_t *output=eval_assign(var, token, NULL); \
	real_type data=0; \
	variable_read(&data, var); \
	const bool pass=( \
		(cmp) && \
		output==(expected_error) \
	); \
	free_types(); \
	free_variable(var); \
	return pass;

#define TEST_EVAL_ASSIGN_FLOAT(str_type, str_value, real_type, expected_val, expected_error) \
	TEST_EVAL_ASSIGN_BASE(str_type, str_value, real_type, expected_val, expected_error, (int)data==(int)(expected_val))

#define TEST_EVAL_ASSIGN(str_type, str_value, real_type, expected_val, expected_error) \
	TEST_EVAL_ASSIGN_BASE(str_type, str_value, real_type, expected_val, expected_error, data==(expected_val))

TEST(eval_assign_int, {
	TEST_EVAL_ASSIGN(L"int", L"1234", int64_t, 1234, NULL);
})

TEST(eval_assign_float, {
	TEST_EVAL_ASSIGN_FLOAT(L"float", L"1234.0", long double, 1234.0, NULL);
})

TEST(eval_assign_bool, {
	TEST_EVAL_ASSIGN(L"bool", L"true", bool, true, NULL);
})

TEST(eval_assign_char, {
	TEST_EVAL_ASSIGN(L"char", L"'a'", wchar_t, L'a', NULL);
})

TEST(eval_assign_str, {
	token_t *token=tokenize(L"\"abc\"");
	classify_tokens(token);

	make_default_types();
	variable_t *var=make_variable(L"str", L"x", false);

	eval_assign(var, token, NULL);

	wchar_t *data=NULL;
	variable_read(&data, var);

	const bool pass=(wcscmp(data, L"abc")==0);

	wchar_t *mem=NULL;
	variable_read(&mem, var);
	free(mem);

	free_types();
	free_variable(var);
	return pass;
})

TEST(eval_assign_int_overflow, {
	TEST_EVAL_ASSIGN(L"int", L"99999999999999999999999999999999", int64_t, 0, ERROR_MSG[ERROR_OVERFLOW]);
})

TEST(eval_assign_type_mismatch, {
	TEST_EVAL_ASSIGN(L"int", L"not_an_int", int64_t, 0, ERROR_MSG[ERROR_TYPE_MISMATCH]);
})

TEST(eval_assign_cannot_assign_non_ints, {
	TEST_EVAL_ASSIGN(L"int", L"3.1415", int64_t, 0, ERROR_MSG[ERROR_TYPE_MISMATCH]);
})

TEST(eval_assign_cannot_assign_non_floats, {
	TEST_EVAL_ASSIGN_FLOAT(L"float", L"1234", int64_t, 0, ERROR_MSG[ERROR_TYPE_MISMATCH]);
})

TEST(eval_assign_cannot_assign_non_bools, {
	TEST_EVAL_ASSIGN(L"bool", L"1", bool, 0, ERROR_MSG[ERROR_TYPE_MISMATCH]);
})

TEST(eval_assign_cannot_assign_non_chars, {
	TEST_EVAL_ASSIGN(L"char", L"1234", wchar_t, 0, ERROR_MSG[ERROR_TYPE_MISMATCH]);
})

TEST(eval_assign_cannot_assign_non_strs, {
	TEST_EVAL_ASSIGN(L"str", L"1234", wchar_t*, 0, ERROR_MSG[ERROR_TYPE_MISMATCH]);
})

#undef TEST_EVAL_ASSIGN

TEST(eval_assign_variable_to_another, {
	make_default_types();
	variable_t *var1=make_variable(L"int", L"var1", false);
	variable_t *var2=make_variable(L"int", L"var2", false);

	int64_t var1_data=0;
	int64_t var2_data=1234;
	variable_write(var1, &var1_data);
	variable_write(var2, &var2_data);

	//assign var2 to var1
	token_t *token=tokenize(L"var2");
	classify_tokens(token);

	context_t *ctx=make_context();
	context_add_var(ctx, var1);
	context_add_var(ctx, var2);

	const wchar_t *output=eval_assign(var1, token, ctx); // NOLINT

	int64_t data=0;
	variable_read(&data, var1);

	const bool pass=(
		output==NULL &&
		data==1234
	);

	free_types();
	free_context(ctx);
	return pass;
})

TEST(eval_assign_variable_to_another_check_same_type, {
	make_default_types();
	variable_t *var1=make_variable(L"int", L"var1", false);
	variable_t *var2=make_variable(L"bool", L"var2", false);

	int64_t var1_data=0;
	bool var2_data=false;
	variable_write(var1, &var1_data);
	variable_write(var2, &var2_data);

	token_t *token=tokenize(L"var2");
	classify_tokens(token);

	context_t *ctx=make_context();
	context_add_var(ctx, var1);
	context_add_var(ctx, var2);

	const bool pass=(wcscmp(
		eval_assign(var1, token, ctx), // NOLINT
		ERROR_MSG[ERROR_TYPE_MISMATCH]
	)==0);

	free_types();
	free_context(ctx);
	return pass;
})

TEST(eval_assign_variable_to_another_check_bad_var, {
	make_default_types();
	variable_t *var=make_variable(L"int", L"var", false);

	int64_t tmp=0;
	variable_write(var, &tmp);

	token_t *token=tokenize(L"not_a_variable");
	classify_tokens(token);

	context_t *ctx=make_context();
	context_add_var(ctx, var);

	const bool pass=(wcscmp(
		eval_assign(var, token, ctx),
		ERROR_MSG[ERROR_INVALID_INPUT]
	)==0);

	free_types();
	free_context(ctx);
	return pass;
})

void eval_assign_test_self(bool *pass) {
	tests_t tests={
		test_eval_assign_int,
		test_eval_assign_float,
		test_eval_assign_bool,
		test_eval_assign_char,
		test_eval_assign_str,
		test_eval_assign_int_overflow,
		test_eval_assign_type_mismatch,
		test_eval_assign_cannot_assign_non_ints,
		test_eval_assign_cannot_assign_non_floats,
		test_eval_assign_cannot_assign_non_bools,
		test_eval_assign_cannot_assign_non_chars,
		test_eval_assign_cannot_assign_non_strs,
		test_eval_assign_variable_to_another,
		test_eval_assign_variable_to_another_check_same_type,
		test_eval_assign_variable_to_another_check_bad_var,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
