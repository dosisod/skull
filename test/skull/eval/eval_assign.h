#include <stdbool.h>

#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/eval/eval_assign.h"
#include "skull/eval/types/defs.h"
#include "skull/parse/classify.h"

#include "test/testing.h"

#define TEST_EVAL_ASSIGN_BASE(str_type, str_value, real_type, expected_val, expected_error, cmp) \
	const char32_t *error = NULL; \
	AstNode *node=make_ast_tree(str_value, &error); \
	Variable *var=make_variable(str_type, U"x", false); \
	const char32_t *output=eval_assign(var, node, NULL); \
	real_type data=0; \
	variable_read(&data, var); \
	const bool pass=( \
		!error && \
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
	const char32_t *error = NULL;
	AstNode *node=make_ast_tree(code, &error);

	Variable *var=make_variable("str", U"x", false);

	eval_assign(var, node, NULL);

	char32_t *data=NULL;
	variable_read(&data, var);

	const bool pass=(
		!error &&
		c32scmp(data, U"abc")
	);

	char32_t *mem=NULL;
	variable_read(&mem, var);
	free(mem);

	free_variable(var);
	return pass;
})

#define TEST_EVAL_ASSIGN_WITH_MATH(data, oper, expected) \
	Scope *scope=make_scope(); \
	Variable *var_a = make_variable("int", U"a", false); \
	SkullInt num = (data); \
	variable_write(var_a, &num); \
	scope_add_var(scope, var_a); \
	const char32_t *error = NULL; \
	AstNode *node = make_ast_tree(U"a " oper U" a", &error); \
	Variable *var_b = make_variable("int", U"b", false); \
	eval_assign(var_b, node, scope); \
	SkullInt out = 0; \
	variable_read(&out, var_b); \
	const bool pass=( \
		!error && \
		out == (expected) \
	); \
	free_variable(var_a); \
	free_variable(var_b); \
	return pass

TEST(eval_assign_add_vars, {
	TEST_EVAL_ASSIGN_WITH_MATH(1, U"+", 2);
})

TEST(eval_assign_sub_vars, {
	TEST_EVAL_ASSIGN_WITH_MATH(1, U"-", 0);
})

TEST(eval_assign_mult_vars, {
	TEST_EVAL_ASSIGN_WITH_MATH(2, U"*", 4);
})

TEST(eval_assign_div_vars, {
	TEST_EVAL_ASSIGN_WITH_MATH(2, U"/", 1);
})

#undef TEST_EVAL_ASSIGN_WITH_MATH

#define TEST_EVAL_ASSIGN_DIFFERENT_TYPES(str_type_a, real_type_a, value_a, oper, str_type_b, real_type_b, value_b, expected) \
	Scope *scope = make_scope(); \
	Variable *var_a = make_variable(str_type_a, U"a", false); \
	real_type_a data_a = (value_a); \
	variable_write(var_a, &data_a); \
	scope_add_var(scope, var_a); \
	Variable *var_b = make_variable(str_type_b, U"b", false); \
	real_type_b data_b = (value_b); \
	variable_write(var_b, &data_b); \
	scope_add_var(scope, var_b); \
	const char32_t *error = NULL; \
	AstNode *node = make_ast_tree(U"a " oper U" b", &error); \
	Variable *var_c = make_variable("int", U"c", false); \
	const char32_t *output = eval_assign(var_c, node, scope); \
	const bool pass=( \
		!error && \
		c32scmp( \
			(expected), \
			output \
		) \
	); \
	free_variable(var_a); \
	free_variable(var_b); \
	return pass

TEST(eval_assign_add_vars_types_must_match, {
	TEST_EVAL_ASSIGN_DIFFERENT_TYPES(
		"int", SkullInt, 1, U"+",
		"rune", SkullRune, 'b',
		ERR_CANNOT_(U"add", U"int", U"rune")
	);
})

TEST(eval_assign_sub_vars_types_must_match, {
	TEST_EVAL_ASSIGN_DIFFERENT_TYPES(
		"int", SkullInt, 1, U"-",
		"rune", SkullRune, 'b',
		ERR_CANNOT_(U"subtract", U"int", U"rune")
	);
})

TEST(eval_assign_mult_vars_types_must_match, {
	TEST_EVAL_ASSIGN_DIFFERENT_TYPES(
		"int", SkullInt, 1, U"*",
		"rune", SkullRune, 'b',
		ERR_CANNOT_(U"multiply", U"int", U"rune")
	);
})

TEST(eval_assign_div_vars_types_must_match, {
	TEST_EVAL_ASSIGN_DIFFERENT_TYPES(
		"int", SkullInt, 1, U"/",
		"rune", SkullRune, 'b',
		ERR_CANNOT_(U"divide", U"int", U"rune")
	);
})

#undef TEST_EVAL_ASSIGN_DIFFERENT_TYPES

#define TEST_EVAL_ASSIGN_VAR_MUST_EXIST(oper) \
	Scope *scope = make_scope(); \
	Variable *var_a = make_variable("int", U"a", false); \
	scope_add_var(scope, var_a); \
	const char32_t *error = NULL; \
	AstNode *node = make_ast_tree(U"a " oper " b", &error); \
	Variable *var_b = make_variable("int", U"b", false); \
	const char32_t *output=eval_assign(var_b, node, scope); \
	const bool pass = ( \
		!error && \
		c32scmp( \
			ERR_VAR_NOT_FOUND_(U"b"), \
			output \
		) \
	); \
	free_variable(var_a); \
	return pass

TEST(eval_assign_add_vars_var_must_exist, {
	TEST_EVAL_ASSIGN_VAR_MUST_EXIST(U"+");
})

TEST(eval_assign_sub_vars_var_must_exist, {
	TEST_EVAL_ASSIGN_VAR_MUST_EXIST(U"-");
})

TEST(eval_assign_mult_vars_var_must_exist, {
	TEST_EVAL_ASSIGN_VAR_MUST_EXIST(U"*");
})

TEST(eval_assign_div_vars_var_must_exist, {
	TEST_EVAL_ASSIGN_VAR_MUST_EXIST(U"/");
})

TEST(eval_assign_check_lhs_var, {
	Scope *scope=make_scope();

	Variable *var_a=make_variable("int", U"a", false);
	SkullInt data_a=1;
	variable_write(var_a, &data_a);
	scope_add_var(scope, var_a);

	const char32_t *error = NULL;
	AstNode *node=make_ast_tree(U"b + a", &error);
	Variable *var_b=make_variable("int", U"b", false);
	const char32_t *output=eval_assign(var_b, node, scope);

	const bool pass=(
		!error &&
		c32scmp(
			ERR_VAR_NOT_FOUND_(U"b"),
			output
		)
	);

	free_variable(var_a);
	return pass;
})

#define TEST_EVAL_ASSIGN_OPER_AVAILABLE(oper, expected) \
	Scope *scope=make_scope(); \
	Variable *var_a = make_variable("bool", U"a", false); \
	scope_add_var(scope, var_a); \
	const char32_t *error = NULL; \
	AstNode *node = make_ast_tree(U"a " oper U" a", &error); \
	Variable *var_b = make_variable("int", U"b", false); \
	const char32_t *output=eval_assign(var_b, node, scope); \
	const bool pass=( \
		!error && \
		c32scmp( \
			(expected), \
			output \
		) \
	); \
	free_variable(var_a); \
	return pass

TEST(eval_assign_add_vars_must_be_addable, {
	TEST_EVAL_ASSIGN_OPER_AVAILABLE(
		U"+",
		ERR_UNAVAILABLE_(U"addition", U"bool")
	);
})

TEST(eval_assign_sub_vars_must_be_subtractable, {
	TEST_EVAL_ASSIGN_OPER_AVAILABLE(
		U"-",
		ERR_UNAVAILABLE_(U"subtraction", U"bool")
	);
})

TEST(eval_assign_mult_vars_must_be_multipliable, {
	TEST_EVAL_ASSIGN_OPER_AVAILABLE(
		U"*",
		ERR_UNAVAILABLE_(U"multiplication", U"bool")
	);
})

TEST(eval_assign_div_vars_must_be_divisible, {
	TEST_EVAL_ASSIGN_OPER_AVAILABLE(
		U"/",
		ERR_UNAVAILABLE_(U"division", U"bool")
	);
})

#undef TEST_EVAL_ASSIGN_OPER_AVAILABLE

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
	const char32_t *error = NULL;
	AstNode *node=make_ast_tree(U"var2", &error);

	Scope *scope=make_scope();
	scope_add_var(scope, var1);
	scope_add_var(scope, var2);

	const char32_t *output=eval_assign(var1, node, scope);

	SkullInt data=0;
	variable_read(&data, var1);

	const bool pass=(
		!error &&
		!output &&
		data==1234
	);

	if (!is_error_msg(output)) {
		free((char32_t *)output);
	}

	free_scope(scope);
	return pass;
})

TEST(eval_assign_variable_to_another_check_same_type, {
	Variable *var1=make_variable("int", U"var1", false);
	Variable *var2=make_variable("bool", U"var2", false);

	SkullInt var1_data=0;
	bool var2_data=false;
	variable_write(var1, &var1_data);
	variable_write(var2, &var2_data);

	const char32_t *error = NULL;
	AstNode *node=make_ast_tree(U"var2", &error);

	Scope *scope=make_scope();
	scope_add_var(scope, var1);
	scope_add_var(scope, var2);

	const char32_t *output=eval_assign(var1, node, scope);

	const bool pass=(
		!error &&
		c32scmp(
			output,
			ERR_TYPE_MISMATCH_(U"int")
		)
	);

	if (!is_error_msg(output)) {
		free((char32_t *)output);
	}

	free_scope(scope);
	return pass;
})

TEST(eval_assign_variable_to_another_check_bad_var, {
	Variable *var=make_variable("int", U"var", false);

	SkullInt tmp=0;
	variable_write(var, &tmp);

	const char32_t *error = NULL;
	AstNode *node=make_ast_tree(U"not_a_variable", &error);

	Scope *scope=make_scope();
	scope_add_var(scope, var);

	const bool pass=(
		!error &&
		c32scmp(
			eval_assign(var, node, scope),
			ERR_VAR_NOT_FOUND_(U"not_a_variable")
		)
	);

	free_scope(scope);
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

	const char32_t *error = NULL;
	AstNode *node=make_ast_tree(U"var2", &error);

	Scope *scope=make_scope();
	scope_add_var(scope, var1);
	scope_add_var(scope, var2);

	const char32_t *output=eval_assign(var1, node, scope);

	char32_t *after_var1=NULL;
	variable_read(&after_var1, var1);

	char32_t *after_var2=NULL;
	variable_read(&after_var2, var2);

	const bool pass=(
		!error &&
		!output &&
		c32scmp(after_var1, str2) &&
		c32scmp(after_var2, str2) &&
		after_var1!=after_var2
	);

	free_scope(scope);
	return pass;
})

TEST(eval_assign_type_type, {
	Variable *var=make_variable("type", U"var", false);

	const char32_t *error = NULL;
	AstNode *node=make_ast_tree(U"int", &error);

	Scope *scope=make_scope();
	const char32_t *output=eval_assign(var, node, scope);

	Type *after=NULL;
	variable_read(&after, var);

	const bool pass=(
		!error &&
		!output &&
		after==&TYPE_INT
	);

	free_scope(scope);
	return pass;
})

TEST(eval_assign_type_var_cannot_be_type, {
	Variable *var=make_variable("type", U"var", false);

	const char32_t *error = NULL;
	AstNode *node=make_ast_tree(U"type", &error);

	Scope *scope=make_scope();

	const bool pass=(
		!error &&
		eval_assign(var, node, scope)==ERR_TYPE_TYPE_BAD
	);

	free_scope(scope);
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
		test_eval_assign_type_type,
		test_eval_assign_type_var_cannot_be_type,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
