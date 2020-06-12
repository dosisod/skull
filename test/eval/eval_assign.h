#include <stdbool.h>

#include "../../src/eval/eval_assign.h"
#include "../../test/testing.h"

TEST(eval_assign_int, {
	token_t *token=tokenize(L"1234");
	classify_tokens(token);

	make_default_types();
	variable_t *var=make_variable(L"int", L"x", false);

	eval_assign(var, token, NULL);

	int64_t data=0;
	variable_read(&data, var);

	const bool pass=(data==1234);

	free_types();
	free_variable(var);
	return pass;
});

TEST(eval_assign_float, {
	token_t *token=tokenize(L"0.0");
	classify_tokens(token);

	make_default_types();
	variable_t *var=make_variable(L"float", L"x", false);

	eval_assign(var, token, NULL);

	long double data=1.0;
	variable_read(&data, var);

	const bool pass=(data==0.0);

	free_types();
	free_variable(var);
	return pass;
});

TEST(eval_assign_bool, {
	token_t *token=tokenize(L"false");
	classify_tokens(token);

	make_default_types();
	variable_t *var=make_variable(L"int", L"x", false);

	eval_assign(var, token, NULL);

	bool data=true;
	variable_read(&data, var);

	const bool pass=(data==false);

	free_types();
	free_variable(var);
	return pass;
});

TEST(eval_assign_char, {
	token_t *token=tokenize(L"'a'");
	classify_tokens(token);

	make_default_types();
	variable_t *var=make_variable(L"char", L"x", false);

	eval_assign(var, token, NULL);

	wchar_t data=L'\0';
	variable_read(&data, var);

	const bool pass=(data==L'a');

	free_types();
	free_variable(var);
	return pass;
});

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
});

TEST(eval_assign_int_overflow, {
	token_t *token=tokenize(L"99999999999999999999999999999999");
	classify_tokens(token);

	make_default_types();
	variable_t *var=make_variable(L"int", L"x", false);

	const wchar_t *output=eval_assign(var, token, NULL);

	const bool pass=(wcscmp(
		output,
		ERROR_MSG[ERROR_OVERFLOW]
	)==0);

	free_types();
	free_variable(var);
	return pass;
});

TEST(eval_assign_type_mismatch, {
	token_t *token=tokenize(L"not_an_int");
	classify_tokens(token);

	make_default_types();
	variable_t *var=make_variable(L"int", L"x", false);

	const wchar_t *output=eval_assign(var, token, NULL);

	const bool pass=(wcscmp(
		output,
		ERROR_MSG[ERROR_TYPE_MISMATCH]
	)==0);

	free_types();
	free_variable(var);
	return pass;
});

TEST(eval_assign_cannot_assign_non_ints, {
	token_t *token=tokenize(L"3.1415");
	classify_tokens(token);

	make_default_types();
	variable_t *var=make_variable(L"int", L"x", false);

	const wchar_t *output=eval_assign(var, token, NULL);

	const bool pass=(wcscmp(
		output,
		ERROR_MSG[ERROR_TYPE_MISMATCH]
	)==0);

	free_types();
	free_variable(var);
	return pass;
});

TEST(eval_assign_cannot_assign_non_floats, {
	token_t *token=tokenize(L"123");
	classify_tokens(token);

	make_default_types();
	variable_t *var=make_variable(L"float", L"x", false);

	const wchar_t *output=eval_assign(var, token, NULL);

	const bool pass=(wcscmp(
		output,
		ERROR_MSG[ERROR_TYPE_MISMATCH]
	)==0);

	free_types();
	free_variable(var);
	return pass;
});

TEST(eval_assign_cannot_assign_non_bools, {
	token_t *token=tokenize(L"1");
	classify_tokens(token);

	make_default_types();
	variable_t *var=make_variable(L"bool", L"x", false);

	const wchar_t *output=eval_assign(var, token, NULL);

	const bool pass=(wcscmp(
		output,
		ERROR_MSG[ERROR_TYPE_MISMATCH]
	)==0);

	free_types();
	free_variable(var);
	return pass;
});

TEST(eval_assign_cannot_assign_non_chars, {
	token_t *token=tokenize(L"1234");
	classify_tokens(token);

	make_default_types();
	variable_t *var=make_variable(L"char", L"x", false);

	const wchar_t *output=eval_assign(var, token, NULL);

	const bool pass=(wcscmp(
		output,
		ERROR_MSG[ERROR_TYPE_MISMATCH]
	)==0);

	free_types();
	free_variable(var);
	return pass;
});

TEST(eval_assign_cannot_assign_non_strs, {
	token_t *token=tokenize(L"1234");
	classify_tokens(token);

	make_default_types();
	variable_t *var=make_variable(L"str", L"x", false);

	const wchar_t *output=eval_assign(var, token, NULL);

	const bool pass=(wcscmp(
		output,
		ERROR_MSG[ERROR_TYPE_MISMATCH]
	)==0);

	wchar_t *mem=NULL;
	variable_read(&mem, var);
	free(mem);

	free_types();
	free_variable(var);
	return pass;
});

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
});

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
});

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
});

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
