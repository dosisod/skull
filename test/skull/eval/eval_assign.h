#include <stdbool.h>

#include "skull/errors.h"
#include "skull/eval/eval_assign.h"
#include "skull/parse/classify.h"

#include "test/testing.h"

#define TEST_EVAL_ASSIGN_BASE(str_type, str_value, real_type, expected_val, expected_error, cmp) \
	ast_node_t *node=make_ast_tree(str_value); \
	variable_t *var=make_variable(str_type, U"x", false); \
	const char32_t *output=eval_assign(var, node, NULL); \
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
	TEST_EVAL_ASSIGN_FLOAT(U"float", U"1234.0", double, 1234.0, NULL);
})

TEST(eval_assign_bool, {
	TEST_EVAL_ASSIGN(U"bool", U"true", bool, true, NULL);
})

TEST(eval_assign_char, {
	TEST_EVAL_ASSIGN(U"char", U"'a'", char32_t, U'a', NULL);
})

TEST(eval_assign_str, {
	const char32_t *code=U"\"abc\"";
	ast_node_t *node=make_ast_tree(code);

	variable_t *var=make_variable(U"str", U"x", false);

	eval_assign(var, node, NULL);

	char32_t *data=NULL;
	variable_read(&data, var);

	const bool pass=c32scmp(data, U"abc");

	char32_t *mem=NULL;
	variable_read(&mem, var);
	free(mem);

	free_variable(var);
	return pass;
})

TEST(eval_assign_add_vars, {
	context_t *ctx=make_context();

	variable_t *var_a=make_variable(U"int", U"a", false);
	int64_t num=1;
	variable_write(var_a, &num);
	context_add_var(ctx, var_a);

	ast_node_t *node=make_ast_tree(U"a + a");
	variable_t *var_b=make_variable(U"int", U"b", false);
	eval_assign(var_b, node, ctx);

	int64_t data=0;
	variable_read(&data, var_b);

	const bool pass=(data==2);

	free_variable(var_a);
	free_variable(var_b);
	return pass;
})

TEST(eval_assign_sub_vars, {
	context_t *ctx=make_context();

	variable_t *var_a=make_variable(U"int", U"a", false);
	int64_t num=1;
	variable_write(var_a, &num);
	context_add_var(ctx, var_a);

	ast_node_t *node=make_ast_tree(U"a - a");
	variable_t *var_b=make_variable(U"int", U"b", false);
	eval_assign(var_b, node, ctx);

	int64_t data=1;
	variable_read(&data, var_b);

	const bool pass=(data==0);

	free_variable(var_a);
	free_variable(var_b);
	return pass;
})

TEST(eval_assign_mult_vars, {
	context_t *ctx=make_context();

	variable_t *var_a=make_variable(U"int", U"a", false);
	int64_t num=2;
	variable_write(var_a, &num);
	context_add_var(ctx, var_a);

	ast_node_t *node=make_ast_tree(U"a * a");
	variable_t *var_b=make_variable(U"int", U"b", false);
	eval_assign(var_b, node, ctx);

	int64_t data=0;
	variable_read(&data, var_b);

	const bool pass=(data==4);

	free_variable(var_a);
	free_variable(var_b);
	return pass;
})

TEST(eval_assign_div_vars, {
	context_t *ctx=make_context();

	variable_t *var_a=make_variable(U"int", U"a", false);
	int64_t num=2;
	variable_write(var_a, &num);
	context_add_var(ctx, var_a);

	ast_node_t *node=make_ast_tree(U"a / a");
	variable_t *var_b=make_variable(U"int", U"b", false);
	eval_assign(var_b, node, ctx);

	int64_t data=0;
	variable_read(&data, var_b);

	const bool pass=(data==1);

	free_variable(var_a);
	free_variable(var_b);
	return pass;
})

TEST(eval_assign_add_vars_types_must_match, {
	context_t *ctx=make_context();

	variable_t *var_a=make_variable(U"int", U"a", false);
	int64_t data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	variable_t *var_b=make_variable(U"char", U"b", false);
	char32_t data_b=U'b';
	variable_write(var_b, &data_b);
	context_add_var(ctx, var_b);

	ast_node_t *node=make_ast_tree(U"a + b");
	variable_t *var_c=make_variable(U"int", U"c", false);
	const char32_t *output=eval_assign(var_c, node, ctx);

	const bool pass=(output==ERR_CANNOT_ADD);

	free_variable(var_a);
	free_variable(var_b);
	return pass;
})

TEST(eval_assign_sub_vars_types_must_match, {
	context_t *ctx=make_context();

	variable_t *var_a=make_variable(U"int", U"a", false);
	int64_t data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	variable_t *var_b=make_variable(U"char", U"b", false);
	char32_t data_b=U'b';
	variable_write(var_b, &data_b);
	context_add_var(ctx, var_b);

	ast_node_t *node=make_ast_tree(U"a - b");
	variable_t *var_c=make_variable(U"int", U"c", false);
	const char32_t *output=eval_assign(var_c, node, ctx);

	const bool pass=(output==ERR_CANNOT_SUB);

	free_variable(var_a);
	free_variable(var_b);
	return pass;
})

TEST(eval_assign_mult_vars_types_must_match, {
	context_t *ctx=make_context();

	variable_t *var_a=make_variable(U"int", U"a", false);
	int64_t data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	variable_t *var_b=make_variable(U"char", U"b", false);
	char32_t data_b=U'b';
	variable_write(var_b, &data_b);
	context_add_var(ctx, var_b);

	ast_node_t *node=make_ast_tree(U"a * b");
	variable_t *var_c=make_variable(U"int", U"c", false);
	const char32_t *output=eval_assign(var_c, node, ctx);

	const bool pass=(output==ERR_CANNOT_MULT);

	free_variable(var_a);
	free_variable(var_b);
	return pass;
})

TEST(eval_assign_div_vars_types_must_match, {
	context_t *ctx=make_context();

	variable_t *var_a=make_variable(U"int", U"a", false);
	int64_t data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	variable_t *var_b=make_variable(U"char", U"b", false);
	char32_t data_b=U'b';
	variable_write(var_b, &data_b);
	context_add_var(ctx, var_b);

	ast_node_t *node=make_ast_tree(U"a / b");
	variable_t *var_c=make_variable(U"int", U"c", false);
	const char32_t *output=eval_assign(var_c, node, ctx);

	const bool pass=(output==ERR_CANNOT_DIV);

	free_variable(var_a);
	free_variable(var_b);
	return pass;
})

TEST(eval_assign_add_vars_var_must_exist, {
	context_t *ctx=make_context();

	variable_t *var_a=make_variable(U"int", U"a", false);
	int64_t data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	ast_node_t *node=make_ast_tree(U"a + b");
	variable_t *var_b=make_variable(U"int", U"b", false);
	const char32_t *output=eval_assign(var_b, node, ctx);

	const bool pass=(output==ERR_VAR_NOT_FOUND);

	free_variable(var_a);
	return pass;
})

TEST(eval_assign_sub_vars_var_must_exist, {
	context_t *ctx=make_context();

	variable_t *var_a=make_variable(U"int", U"a", false);
	int64_t data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	ast_node_t *node=make_ast_tree(U"a - b");
	variable_t *var_b=make_variable(U"int", U"b", false);
	const char32_t *output=eval_assign(var_b, node, ctx);

	const bool pass=(output==ERR_VAR_NOT_FOUND);

	free_variable(var_a);
	return pass;
})

TEST(eval_assign_mult_vars_var_must_exist, {
	context_t *ctx=make_context();

	variable_t *var_a=make_variable(U"int", U"a", false);
	int64_t data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	ast_node_t *node=make_ast_tree(U"a * b");
	variable_t *var_b=make_variable(U"int", U"b", false);
	const char32_t *output=eval_assign(var_b, node, ctx);

	const bool pass=(output==ERR_VAR_NOT_FOUND);

	free_variable(var_a);
	return pass;
})

TEST(eval_assign_div_vars_var_must_exist, {
	context_t *ctx=make_context();

	variable_t *var_a=make_variable(U"int", U"a", false);
	int64_t data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	ast_node_t *node=make_ast_tree(U"a / b");
	variable_t *var_b=make_variable(U"int", U"b", false);
	const char32_t *output=eval_assign(var_b, node, ctx);

	const bool pass=(output==ERR_VAR_NOT_FOUND);

	free_variable(var_a);
	return pass;
})

TEST(eval_assign_add_vars_must_be_addable, {
	context_t *ctx=make_context();

	variable_t *var_a=make_variable(U"bool", U"a", false);
	int64_t data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	ast_node_t *node=make_ast_tree(U"a + a");
	variable_t *var_b=make_variable(U"int", U"b", false);
	const char32_t *output=eval_assign(var_b, node, ctx);

	const bool pass=(output==ERR_ADD_UNAVAILABLE);

	free_variable(var_a);
	return pass;
})

TEST(eval_assign_sub_vars_must_be_subtractable, {
	context_t *ctx=make_context();

	variable_t *var_a=make_variable(U"bool", U"a", false);
	int64_t data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	ast_node_t *node=make_ast_tree(U"a - a");
	variable_t *var_b=make_variable(U"int", U"b", false);
	const char32_t *output=eval_assign(var_b, node, ctx);

	const bool pass=(output==ERR_SUB_UNAVAILABLE);

	free_variable(var_a);
	return pass;
})

TEST(eval_assign_mult_vars_must_be_multipliable, {
	context_t *ctx=make_context();

	variable_t *var_a=make_variable(U"bool", U"a", false);
	int64_t data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	ast_node_t *node=make_ast_tree(U"a * a");
	variable_t *var_b=make_variable(U"int", U"b", false);
	const char32_t *output=eval_assign(var_b, node, ctx);

	const bool pass=(output==ERR_MULT_UNAVAILABLE);

	free_variable(var_a);
	return pass;
})

TEST(eval_assign_div_vars_must_be_divisible, {
	context_t *ctx=make_context();

	variable_t *var_a=make_variable(U"bool", U"a", false);
	int64_t data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	ast_node_t *node=make_ast_tree(U"a / a");
	variable_t *var_b=make_variable(U"int", U"b", false);
	const char32_t *output=eval_assign(var_b, node, ctx);

	const bool pass=(output==ERR_DIV_UNAVAILABLE);

	free_variable(var_a);
	return pass;
})

TEST(eval_assign_int_overflow, {
	TEST_EVAL_ASSIGN(U"int", U"99999999999999999999999999999999", int64_t, 0, ERR_OVERFLOW);
})

TEST(eval_assign_type_mismatch, {
	TEST_EVAL_ASSIGN(U"int", U"not_an_int", int64_t, 0, ERR_TYPE_MISMATCH);
})

TEST(eval_assign_cannot_assign_non_ints, {
	TEST_EVAL_ASSIGN(U"int", U"3.1415", int64_t, 0, ERR_TYPE_MISMATCH);
})

TEST(eval_assign_cannot_assign_non_floats, {
	TEST_EVAL_ASSIGN_FLOAT(U"float", U"1234", double, 0, ERR_TYPE_MISMATCH);
})

TEST(eval_assign_cannot_assign_non_bools, {
	TEST_EVAL_ASSIGN(U"bool", U"1", bool, 0, ERR_TYPE_MISMATCH);
})

TEST(eval_assign_cannot_assign_non_chars, {
	TEST_EVAL_ASSIGN(U"char", U"1234", char32_t, 0, ERR_TYPE_MISMATCH);
})

TEST(eval_assign_cannot_assign_non_strs, {
	TEST_EVAL_ASSIGN(U"str", U"1234", char32_t*, 0, ERR_TYPE_MISMATCH);
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
	ast_node_t *node=make_ast_tree(U"var2");

	context_t *ctx=make_context();
	context_add_var(ctx, var1);
	context_add_var(ctx, var2);

	const char32_t *output=eval_assign(var1, node, ctx);

	int64_t data=0;
	variable_read(&data, var1);

	const bool pass=(
		!output &&
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

	ast_node_t *node=make_ast_tree(U"var2");

	context_t *ctx=make_context();
	context_add_var(ctx, var1);
	context_add_var(ctx, var2);

	const char32_t *output=eval_assign(var1, node, ctx);

	const bool pass=c32scmp(
		output,
		ERR_TYPE_MISMATCH
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

	ast_node_t *node=make_ast_tree(U"not_a_variable");

	context_t *ctx=make_context();
	context_add_var(ctx, var);

	const bool pass=c32scmp(
		eval_assign(var, node, ctx),
		ERR_VAR_NOT_FOUND
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

	ast_node_t *node=make_ast_tree(U"var2");

	context_t *ctx=make_context();
	context_add_var(ctx, var1);
	context_add_var(ctx, var2);

	const char32_t *output=eval_assign(var1, node, ctx);

	char32_t *after_var1=NULL;
	variable_read(&after_var1, var1);

	char32_t *after_var2=NULL;
	variable_read(&after_var2, var2);

	const bool pass=(
		!output &&
		c32scmp(after_var1, str2) &&
		c32scmp(after_var2, str2) &&
		after_var1!=after_var2
	);

	free_context(ctx);
	return pass;
})

TEST(eval_assign_type_type, {
	variable_t *var=make_variable(U"type", U"var", false);

	ast_node_t *node=make_ast_tree(U"int");

	context_t *ctx=make_context();
	const char32_t *output=eval_assign(var, node, ctx);

	type_t *after=NULL;
	variable_read(&after, var);

	const bool pass=(
		!output &&
		after==&TYPE_INT
	);

	free_context(ctx);
	return pass;
})

TEST(eval_assign_type_var_cannot_be_type, {
	variable_t *var=make_variable(U"type", U"var", false);

	ast_node_t *node=make_ast_tree(U"type");

	context_t *ctx=make_context();

	const bool pass=(
		eval_assign(var, node, ctx)==ERR_TYPE_TYPE_BAD
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
		test_eval_assign_add_vars,
		test_eval_assign_add_vars_types_must_match,
		test_eval_assign_add_vars_var_must_exist,
		test_eval_assign_add_vars_must_be_addable,
		test_eval_assign_sub_vars,
		test_eval_assign_sub_vars_types_must_match,
		test_eval_assign_sub_vars_var_must_exist,
		test_eval_assign_sub_vars_must_be_subtractable,
		test_eval_assign_sub_vars,
		test_eval_assign_mult_vars,
		test_eval_assign_mult_vars_types_must_match,
		test_eval_assign_mult_vars_var_must_exist,
		test_eval_assign_mult_vars_must_be_multipliable,
		test_eval_assign_div_vars,
		test_eval_assign_div_vars_types_must_match,
		test_eval_assign_div_vars_var_must_exist,
		test_eval_assign_div_vars_must_be_divisible,
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
		test_eval_assign_type_type,
		test_eval_assign_type_var_cannot_be_type,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
