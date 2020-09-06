#include <stdbool.h>

#include "skull/errors.h"
#include "skull/eval/eval_assign.h"
#include "skull/eval/types/defs.h"
#include "skull/parse/classify.h"

#include "test/testing.h"

#define TEST_EVAL_ASSIGN_BASE(str_type, str_value, real_type, expected_val, expected_error, cmp) \
	AstNode *node=make_ast_tree(str_value); \
	Variable *var=make_variable(str_type, U"x", false); \
	const char32_t *output=eval_assign(var, node, NULL); \
	real_type data=0; \
	variable_read(&data, var); \
	const bool pass=( \
		(cmp) && \
		(output==(expected_error) || c32scmp(expected_error, output)) \
	); \
	free_variable(var); \
	return pass;

#define TEST_EVAL_ASSIGN_FLOAT(str_type, str_value, real_type, expected_val, expected_error) \
	TEST_EVAL_ASSIGN_BASE(str_type, str_value, real_type, expected_val, expected_error, (int)data==(int)(expected_val))

#define TEST_EVAL_ASSIGN(str_type, str_value, real_type, expected_val, expected_error) \
	TEST_EVAL_ASSIGN_BASE(str_type, str_value, real_type, expected_val, expected_error, data==(expected_val))

TEST(eval_assign_int, {
	TEST_EVAL_ASSIGN("int", U"1234", SkullInt, 1234, NULL);
})

TEST(eval_assign_float, {
	TEST_EVAL_ASSIGN_FLOAT("float", U"1234.0", SkullFloat, 1234.0, NULL);
})

TEST(eval_assign_bool, {
	TEST_EVAL_ASSIGN("bool", U"true", bool, true, NULL);
})

TEST(eval_assign_rune, {
	TEST_EVAL_ASSIGN("rune", U"'a'", SkullRune, 'a', NULL);
})

TEST(eval_assign_rune_escaped, {
	TEST_EVAL_ASSIGN("rune", U"'\\n'", SkullRune, '\n', NULL);
})

TEST(eval_assign_str, {
	const char32_t *code=U"\"abc\"";
	AstNode *node=make_ast_tree(code);

	Variable *var=make_variable("str", U"x", false);

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
	Context *ctx=make_context();

	Variable *var_a=make_variable("int", U"a", false);
	SkullInt num=1;
	variable_write(var_a, &num);
	context_add_var(ctx, var_a);

	AstNode *node=make_ast_tree(U"a + a");
	Variable *var_b=make_variable("int", U"b", false);
	eval_assign(var_b, node, ctx);

	SkullInt data=0;
	variable_read(&data, var_b);

	const bool pass=(data==2);

	free_variable(var_a);
	free_variable(var_b);
	return pass;
})

TEST(eval_assign_sub_vars, {
	Context *ctx=make_context();

	Variable *var_a=make_variable("int", U"a", false);
	SkullInt num=1;
	variable_write(var_a, &num);
	context_add_var(ctx, var_a);

	AstNode *node=make_ast_tree(U"a - a");
	Variable *var_b=make_variable("int", U"b", false);
	eval_assign(var_b, node, ctx);

	SkullInt data=1;
	variable_read(&data, var_b);

	const bool pass=(data==0);

	free_variable(var_a);
	free_variable(var_b);
	return pass;
})

TEST(eval_assign_mult_vars, {
	Context *ctx=make_context();

	Variable *var_a=make_variable("int", U"a", false);
	SkullInt num=2;
	variable_write(var_a, &num);
	context_add_var(ctx, var_a);

	AstNode *node=make_ast_tree(U"a * a");
	Variable *var_b=make_variable("int", U"b", false);
	eval_assign(var_b, node, ctx);

	SkullInt data=0;
	variable_read(&data, var_b);

	const bool pass=(data==4);

	free_variable(var_a);
	free_variable(var_b);
	return pass;
})

TEST(eval_assign_div_vars, {
	Context *ctx=make_context();

	Variable *var_a=make_variable("int", U"a", false);
	SkullInt num=2;
	variable_write(var_a, &num);
	context_add_var(ctx, var_a);

	AstNode *node=make_ast_tree(U"a / a");
	Variable *var_b=make_variable("int", U"b", false);
	eval_assign(var_b, node, ctx);

	SkullInt data=0;
	variable_read(&data, var_b);

	const bool pass=(data==1);

	free_variable(var_a);
	free_variable(var_b);
	return pass;
})

TEST(eval_assign_add_vars_types_must_match, {
	Context *ctx=make_context();

	Variable *var_a=make_variable("int", U"a", false);
	SkullInt data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	Variable *var_b=make_variable("rune", U"b", false);
	SkullRune data_b='b';
	variable_write(var_b, &data_b);
	context_add_var(ctx, var_b);

	AstNode *node=make_ast_tree(U"a + b");
	Variable *var_c=make_variable("int", U"c", false);
	const char32_t *output=eval_assign(var_c, node, ctx);

	const bool pass=c32scmp(
		ERR_CANNOT_(U"add", U"int", U"rune"),
		output
	);

	free_variable(var_a);
	free_variable(var_b);
	return pass;
})

TEST(eval_assign_sub_vars_types_must_match, {
	Context *ctx=make_context();

	Variable *var_a=make_variable("int", U"a", false);
	SkullInt data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	Variable *var_b=make_variable("rune", U"b", false);
	SkullRune data_b='b';
	variable_write(var_b, &data_b);
	context_add_var(ctx, var_b);

	AstNode *node=make_ast_tree(U"a - b");
	Variable *var_c=make_variable("int", U"c", false);
	const char32_t *output=eval_assign(var_c, node, ctx);

	const bool pass=c32scmp(
		ERR_CANNOT_(U"subtract", U"int", U"rune"),
		output
	);

	free_variable(var_a);
	free_variable(var_b);
	return pass;
})

TEST(eval_assign_mult_vars_types_must_match, {
	Context *ctx=make_context();

	Variable *var_a=make_variable("int", U"a", false);
	SkullInt data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	Variable *var_b=make_variable("rune", U"b", false);
	SkullRune data_b='b';
	variable_write(var_b, &data_b);
	context_add_var(ctx, var_b);

	AstNode *node=make_ast_tree(U"a * b");
	Variable *var_c=make_variable("int", U"c", false);
	const char32_t *output=eval_assign(var_c, node, ctx);

	const bool pass=c32scmp(
		ERR_CANNOT_(U"multiply", U"int", U"rune"),
		output
	);

	free_variable(var_a);
	free_variable(var_b);
	return pass;
})

TEST(eval_assign_div_vars_types_must_match, {
	Context *ctx=make_context();

	Variable *var_a=make_variable("int", U"a", false);
	SkullInt data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	Variable *var_b=make_variable("rune", U"b", false);
	SkullRune data_b='b';
	variable_write(var_b, &data_b);
	context_add_var(ctx, var_b);

	AstNode *node=make_ast_tree(U"a / b");
	Variable *var_c=make_variable("int", U"c", false);
	const char32_t *output=eval_assign(var_c, node, ctx);

	const bool pass=c32scmp(
		ERR_CANNOT_(U"divide", U"int", U"rune"),
		output
	);

	free_variable(var_a);
	free_variable(var_b);
	return pass;
})

TEST(eval_assign_add_vars_var_must_exist, {
	Context *ctx=make_context();

	Variable *var_a=make_variable("int", U"a", false);
	SkullInt data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	AstNode *node=make_ast_tree(U"a + b");
	Variable *var_b=make_variable("int", U"b", false);
	const char32_t *output=eval_assign(var_b, node, ctx);

	const bool pass=c32scmp(
		ERR_VAR_NOT_FOUND_(U"b"),
		output
	);

	free_variable(var_a);
	return pass;
})

TEST(eval_assign_sub_vars_var_must_exist, {
	Context *ctx=make_context();

	Variable *var_a=make_variable("int", U"a", false);
	SkullInt data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	AstNode *node=make_ast_tree(U"a - b");
	Variable *var_b=make_variable("int", U"b", false);
	const char32_t *output=eval_assign(var_b, node, ctx);

	const bool pass=c32scmp(
		ERR_VAR_NOT_FOUND_(U"b"),
		output
	);

	free_variable(var_a);
	return pass;
})

TEST(eval_assign_mult_vars_var_must_exist, {
	Context *ctx=make_context();

	Variable *var_a=make_variable("int", U"a", false);
	SkullInt data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	AstNode *node=make_ast_tree(U"a * b");
	Variable *var_b=make_variable("int", U"b", false);
	const char32_t *output=eval_assign(var_b, node, ctx);

	const bool pass=c32scmp(
		ERR_VAR_NOT_FOUND_(U"b"),
		output
	);

	free_variable(var_a);
	return pass;
})

TEST(eval_assign_div_vars_var_must_exist, {
	Context *ctx=make_context();

	Variable *var_a=make_variable("int", U"a", false);
	SkullInt data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	AstNode *node=make_ast_tree(U"a / b");
	Variable *var_b=make_variable("int", U"b", false);
	const char32_t *output=eval_assign(var_b, node, ctx);

	const bool pass=c32scmp(
		ERR_VAR_NOT_FOUND_(U"b"),
		output
	);

	free_variable(var_a);
	return pass;
})

TEST(eval_assign_check_lhs_var, {
	Context *ctx=make_context();

	Variable *var_a=make_variable("int", U"a", false);
	SkullInt data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	AstNode *node=make_ast_tree(U"b + a");
	Variable *var_b=make_variable("int", U"b", false);
	const char32_t *output=eval_assign(var_b, node, ctx);

	const bool pass=c32scmp(
		ERR_VAR_NOT_FOUND_(U"b"),
		output
	);

	free_variable(var_a);
	return pass;
})

TEST(eval_assign_add_vars_must_be_addable, {
	Context *ctx=make_context();

	Variable *var_a=make_variable("bool", U"a", false);
	SkullInt data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	AstNode *node=make_ast_tree(U"a + a");
	Variable *var_b=make_variable("int", U"b", false);
	const char32_t *output=eval_assign(var_b, node, ctx);

	const bool pass=c32scmp(
		ERR_UNAVAILABLE_(U"addition", U"bool"),
		output
	);

	free_variable(var_a);
	return pass;
})

TEST(eval_assign_sub_vars_must_be_subtractable, {
	Context *ctx=make_context();

	Variable *var_a=make_variable("bool", U"a", false);
	SkullInt data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	AstNode *node=make_ast_tree(U"a - a");
	Variable *var_b=make_variable("int", U"b", false);
	const char32_t *output=eval_assign(var_b, node, ctx);

	const bool pass=c32scmp(
		ERR_UNAVAILABLE_(U"subtraction", U"bool"),
		output
	);

	free_variable(var_a);
	return pass;
})

TEST(eval_assign_mult_vars_must_be_multipliable, {
	Context *ctx=make_context();

	Variable *var_a=make_variable("bool", U"a", false);
	SkullInt data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	AstNode *node=make_ast_tree(U"a * a");
	Variable *var_b=make_variable("int", U"b", false);
	const char32_t *output=eval_assign(var_b, node, ctx);

	const bool pass=c32scmp(
		ERR_UNAVAILABLE_(U"multiplication", U"bool"),
		output
	);

	free_variable(var_a);
	return pass;
})

TEST(eval_assign_div_vars_must_be_divisible, {
	Context *ctx=make_context();

	Variable *var_a=make_variable("bool", U"a", false);
	SkullInt data_a=1;
	variable_write(var_a, &data_a);
	context_add_var(ctx, var_a);

	AstNode *node=make_ast_tree(U"a / a");
	Variable *var_b=make_variable("int", U"b", false);
	const char32_t *output=eval_assign(var_b, node, ctx);

	const bool pass=c32scmp(
		ERR_UNAVAILABLE_(U"division", U"bool"),
		output
	);

	free_variable(var_a);
	return pass;
})

#define TEMP_INT U"99999999999999999999999999999999"
TEST(eval_assign_int_overflow, {
	TEST_EVAL_ASSIGN(
		"int",
		TEMP_INT,
		SkullInt,
		0,
		ERR_OVERFLOW_(TEMP_INT)
	);
})
#undef TEMP_INT

TEST(eval_assign_type_mismatch, {
	TEST_EVAL_ASSIGN("int", U"not_an_int", SkullInt, 0, ERR_TYPE_MISMATCH_(U"int"));
})

TEST(eval_assign_cannot_assign_non_ints, {
	TEST_EVAL_ASSIGN("int", U"3.1415", SkullInt, 0, ERR_TYPE_MISMATCH_(U"int"));
})

TEST(eval_assign_cannot_assign_non_floats, {
	TEST_EVAL_ASSIGN_FLOAT("float", U"1234", SkullFloat, 0, ERR_TYPE_MISMATCH_(U"float"));
})

TEST(eval_assign_cannot_assign_non_bools, {
	TEST_EVAL_ASSIGN("bool", U"1", bool, 0, ERR_TYPE_MISMATCH_(U"bool"));
})

TEST(eval_assign_cannot_assign_non_runes, {
	TEST_EVAL_ASSIGN("rune", U"1234", SkullRune, 0, ERR_TYPE_MISMATCH_(U"rune"));
})

TEST(eval_assign_cannot_assign_non_strs, {
	TEST_EVAL_ASSIGN("str", U"1234", SkullStr, 0, ERR_TYPE_MISMATCH_(U"str"));
})

#undef TEST_EVAL_ASSIGN

TEST(eval_assign_variable_to_another, {
	Variable *var1=make_variable("int", U"var1", false);
	Variable *var2=make_variable("int", U"var2", false);

	SkullInt var1_data=0;
	SkullInt var2_data=1234;
	variable_write(var1, &var1_data);
	variable_write(var2, &var2_data);

	//assign var2 to var1
	AstNode *node=make_ast_tree(U"var2");

	Context *ctx=make_context();
	context_add_var(ctx, var1);
	context_add_var(ctx, var2);

	const char32_t *output=eval_assign(var1, node, ctx);

	SkullInt data=0;
	variable_read(&data, var1);

	const bool pass=(
		!output &&
		data==1234
	);

	if (!is_error_msg(output)) {
		free((char32_t *)output);
	}

	free_context(ctx);
	return pass;
})

TEST(eval_assign_variable_to_another_check_same_type, {
	Variable *var1=make_variable("int", U"var1", false);
	Variable *var2=make_variable("bool", U"var2", false);

	SkullInt var1_data=0;
	bool var2_data=false;
	variable_write(var1, &var1_data);
	variable_write(var2, &var2_data);

	AstNode *node=make_ast_tree(U"var2");

	Context *ctx=make_context();
	context_add_var(ctx, var1);
	context_add_var(ctx, var2);

	const char32_t *output=eval_assign(var1, node, ctx);

	const bool pass=c32scmp(
		output,
		ERR_TYPE_MISMATCH_(U"int")
	);

	if (!is_error_msg(output)) {
		free((char32_t *)output);
	}

	free_context(ctx);
	return pass;
})

TEST(eval_assign_variable_to_another_check_bad_var, {
	Variable *var=make_variable("int", U"var", false);

	SkullInt tmp=0;
	variable_write(var, &tmp);

	AstNode *node=make_ast_tree(U"not_a_variable");

	Context *ctx=make_context();
	context_add_var(ctx, var);

	const bool pass=c32scmp(
		eval_assign(var, node, ctx),
		ERR_VAR_NOT_FOUND_(U"not_a_variable")
	);

	free_context(ctx);
	return pass;
})

TEST(eval_assign_string_types_cannot_share_pointers, {
	Variable *var1=make_variable("str", U"var1", false);
	Variable *var2=make_variable("str", U"var2", false);

	const char32_t *str1=NULL;
	variable_write(var1, &str1);

	const char32_t *str2=c32sdup(U"anything");
	variable_write(var2, &str2);
	var2->is_const=true;

	AstNode *node=make_ast_tree(U"var2");

	Context *ctx=make_context();
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

TEST(eval_assign_Typeype, {
	Variable *var=make_variable("type", U"var", false);

	AstNode *node=make_ast_tree(U"int");

	Context *ctx=make_context();
	const char32_t *output=eval_assign(var, node, ctx);

	Type *after=NULL;
	variable_read(&after, var);

	const bool pass=(
		!output &&
		after==&TYPE_INT
	);

	free_context(ctx);
	return pass;
})

TEST(eval_assign_type_var_cannot_be_type, {
	Variable *var=make_variable("type", U"var", false);

	AstNode *node=make_ast_tree(U"type");

	Context *ctx=make_context();

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
		test_eval_assign_rune,
		test_eval_assign_rune_escaped,
		test_eval_assign_str,
		test_eval_assign_add_vars,
		test_eval_assign_add_vars_types_must_match,
		test_eval_assign_add_vars_var_must_exist,
		test_eval_assign_check_lhs_var,
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
		test_eval_assign_cannot_assign_non_runes,
		test_eval_assign_cannot_assign_non_strs,
		test_eval_assign_variable_to_another,
		test_eval_assign_variable_to_another_check_same_type,
		test_eval_assign_variable_to_another_check_bad_var,
		test_eval_assign_string_types_cannot_share_pointers,
		test_eval_assign_Typeype,
		test_eval_assign_type_var_cannot_be_type,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
