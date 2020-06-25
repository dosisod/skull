#include <stdbool.h>

#include "../../src/errors.h"
#include "../../src/eval/eval_assign.h"
#include "../../src/parse/classify.h"
#include "../../test/testing.h"

#define TEST_EVAL_ASSIGN_BASE(str_type, str_value, real_type, expected_val, expected_error, cmp) \
	token_t *token=tokenize(str_value); \
	classify_tokens(token); \
	variable_t *var=make_variable(str_type, U"x", false); \
	const char32_t *output=eval_assign(var, token, NULL); \
	real_type data=0; \
	variable_read(&data, var); \
	const bool pass=( \
		(cmp) && \
		output==(expected_error) \
	); \
	free_variable(var); \
	return pass;

#define TEST_EVAL_ASSIGN_FLOAT(str_type, str_value, real_type, expected_val, expected_error) \
	TEST_EVAL_ASSIGN_BASE(str_type, str_value, real_type, expected_val, expected_error, (int)data==(int)(expected_val))

#define TEST_EVAL_ASSIGN(str_type, str_value, real_type, expected_val, expected_error) \
	TEST_EVAL_ASSIGN_BASE(str_type, str_value, real_type, expected_val, expected_error, data==(expected_val))

TEST(eval_assign_int, {
	TEST_EVAL_ASSIGN(U"int", U"1234", int64_t, 1234, NULL);
})

TEST(eval_assign_float, {
	TEST_EVAL_ASSIGN_FLOAT(U"float", U"1234.0", long double, 1234.0, NULL);
})

TEST(eval_assign_bool, {
	TEST_EVAL_ASSIGN(U"bool", U"true", bool, true, NULL);
})

TEST(eval_assign_char, {
	TEST_EVAL_ASSIGN(U"char", U"'a'", char32_t, U'a', NULL);
})

TEST(eval_assign_str, {
	token_t *token=tokenize(U"\"abc\"");
	classify_tokens(token);

	variable_t *var=make_variable(U"str", U"x", false);

	eval_assign(var, token, NULL);

	char32_t *data=NULL;
	variable_read(&data, var);

	const bool pass=c32scmp(data, U"abc");

	char32_t *mem=NULL;
	variable_read(&mem, var);
	free(mem);

	free_variable(var);
	return pass;
})

TEST(eval_assign_int_overflow, {
	TEST_EVAL_ASSIGN(U"int", U"99999999999999999999999999999999", int64_t, 0, ERROR_MSG[ERROR_OVERFLOW]);
})

TEST(eval_assign_type_mismatch, {
	TEST_EVAL_ASSIGN(U"int", U"not_an_int", int64_t, 0, ERROR_MSG[ERROR_TYPE_MISMATCH]);
})

TEST(eval_assign_cannot_assign_non_ints, {
	TEST_EVAL_ASSIGN(U"int", U"3.1415", int64_t, 0, ERROR_MSG[ERROR_TYPE_MISMATCH]);
})

TEST(eval_assign_cannot_assign_non_floats, {
	TEST_EVAL_ASSIGN_FLOAT(U"float", U"1234", long double, 0, ERROR_MSG[ERROR_TYPE_MISMATCH]);
})

TEST(eval_assign_cannot_assign_non_bools, {
	TEST_EVAL_ASSIGN(U"bool", U"1", bool, 0, ERROR_MSG[ERROR_TYPE_MISMATCH]);
})

TEST(eval_assign_cannot_assign_non_chars, {
	TEST_EVAL_ASSIGN(U"char", U"1234", char32_t, 0, ERROR_MSG[ERROR_TYPE_MISMATCH]);
})

TEST(eval_assign_cannot_assign_non_strs, {
	TEST_EVAL_ASSIGN(U"str", U"1234", char32_t*, 0, ERROR_MSG[ERROR_TYPE_MISMATCH]);
})

#undef TEST_EVAL_ASSIGN

TEST(eval_assign_variable_to_another, {
	variable_t *var1=make_variable(U"int", U"var1", false);
	variable_t *var2=make_variable(U"int", U"var2", false);

	int64_t var1_data=0;
	int64_t var2_data=1234;
	variable_write(var1, &var1_data);
	variable_write(var2, &var2_data);

	//assign var2 to var1
	token_t *token=tokenize(U"var2");
	classify_tokens(token);

	context_t *ctx=make_context();
	context_add_var(ctx, var1);
	context_add_var(ctx, var2);

	const char32_t *output=eval_assign(var1, token, ctx); // NOLINT

	int64_t data=0;
	variable_read(&data, var1);

	const bool pass=(
		output==NULL &&
		data==1234
	);

	if (!is_error_msg(output)) {
		free((char32_t*)output);
	}

	free_context(ctx);
	return pass;
})

TEST(eval_assign_variable_to_another_check_same_type, {
	variable_t *var1=make_variable(U"int", U"var1", false);
	variable_t *var2=make_variable(U"bool", U"var2", false);

	int64_t var1_data=0;
	bool var2_data=false;
	variable_write(var1, &var1_data);
	variable_write(var2, &var2_data);

	token_t *token=tokenize(U"var2");
	classify_tokens(token);

	context_t *ctx=make_context();
	context_add_var(ctx, var1);
	context_add_var(ctx, var2);

	const char32_t *output=eval_assign(var1, token, ctx); // NOLINT

	const bool pass=c32scmp(
		output,
		ERROR_MSG[ERROR_TYPE_MISMATCH]
	);

	if (!is_error_msg(output)) {
		free((char32_t*)output);
	}

	free_context(ctx);
	return pass;
})

TEST(eval_assign_variable_to_another_check_bad_var, {
	variable_t *var=make_variable(U"int", U"var", false);

	int64_t tmp=0;
	variable_write(var, &tmp);

	token_t *token=tokenize(U"not_a_variable");
	classify_tokens(token);

	context_t *ctx=make_context();
	context_add_var(ctx, var);

	const bool pass=c32scmp(
		eval_assign(var, token, ctx),
		ERROR_MSG[ERROR_INVALID_INPUT]
	);

	free_context(ctx);
	return pass;
})

TEST(eval_assign_string_types_cannot_share_pointers, {
	variable_t *var1=make_variable(U"str", U"var1", false);
	variable_t *var2=make_variable(U"str", U"var2", false);

	const char32_t *str1=NULL;
	variable_write(var1, &str1);

	const char32_t *str2=c32sdup(U"anything");
	variable_write(var2, &str2);
	var2->is_const=true;

	token_t *token=tokenize(U"var2");
	classify_tokens(token);

	context_t *ctx=make_context();
	context_add_var(ctx, var1);
	context_add_var(ctx, var2);

	const char32_t *output=eval_assign(var1, token, ctx);

	char32_t *after_var1=NULL;
	variable_read(&after_var1, var1);

	char32_t *after_var2=NULL;
	variable_read(&after_var2, var2);

	const bool pass=(
		output==NULL &&
		c32scmp(after_var1, str2) &&
		c32scmp(after_var2, str2) && // NOLINT
		after_var1!=after_var2
	);

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
		test_eval_assign_string_types_cannot_share_pointers,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
