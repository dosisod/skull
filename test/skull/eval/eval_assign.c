#include <stdbool.h>

#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/eval/eval_assign.h"
#include "skull/eval/types/defs.h"
#include "skull/parse/classify.h"

#include "test/skull/eval/eval_assign.h"
#include "test/testing.h"

TEST(eval_assign_int, {
	TEST_EVAL_ASSIGN("int", U"1234", SkullInt, 1234, NULL);
})

TEST(eval_assign_float, {
	TEST_EVAL_ASSIGN_FLOAT("float", U"1234.0", SkullFloat, 1234.0, NULL); // NOLINT
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
	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"\"abc\"", &error);
	ASSERT_FALSEY(error);

	Variable *var = make_variable("str", U"x", false);

	eval_assign(var, node, NULL);

	char32_t *data = NULL;
	variable_read(&data, var);

	ASSERT_TRUTHY(c32scmp(data, U"abc"));

	char32_t *mem = NULL;
	variable_read(&mem, var);
	free(mem);
	free_variable(var);
})

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
	Scope *scope = make_scope();

	Variable *var_a = make_variable("int", U"a", false);
	SkullInt data_a = 1;
	variable_write(var_a, &data_a);
	scope_add_var(scope, var_a);

	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"b + a", &error);
	ASSERT_FALSEY(error);

	Variable *var_b = make_variable("int", U"b", false);
	const char32_t *output = eval_assign(var_b, node, scope);

	ASSERT_TRUTHY(c32scmp(
		ERR_VAR_NOT_FOUND_(U"b"),
		output
	));

	free_variable(var_a);
})

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

TEST(eval_assign_variable_to_another, {
	Variable *var1 = make_variable("int", U"var1", false);
	Variable *var2 = make_variable("int", U"var2", false);

	SkullInt var1_data = 0;
	SkullInt var2_data = 1234;
	variable_write(var1, &var1_data);
	variable_write(var2, &var2_data);

	//assign var2 to var1
	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"var2", &error);
	ASSERT_FALSEY(error);

	Scope *scope = make_scope();
	scope_add_var(scope, var1);
	scope_add_var(scope, var2);

	const char32_t *output = eval_assign(var1, node, scope);
	ASSERT_FALSEY(output);

	SkullInt data = 0;
	variable_read(&data, var1);

	ASSERT_EQUAL(data, 1234);

	if (!is_error_msg(output)) {
		free((char32_t *)output);
	}

	free_scope(scope);
})

TEST(eval_assign_variable_to_another_check_same_type, {
	Variable *var1 = make_variable("int", U"var1", false);
	Variable *var2 = make_variable("bool", U"var2", false);

	SkullInt var1_data = 0;
	bool var2_data = false;
	variable_write(var1, &var1_data);
	variable_write(var2, &var2_data);

	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"var2", &error);
	ASSERT_FALSEY(error);

	Scope *scope = make_scope();
	scope_add_var(scope, var1);
	scope_add_var(scope, var2);

	const char32_t *output = eval_assign(var1, node, scope);

	ASSERT_TRUTHY(c32scmp(
		output,
		ERR_TYPE_MISMATCH_(U"int")
	));

	if (!is_error_msg(output)) {
		free((char32_t *)output);
	}

	free_scope(scope);
})

TEST(eval_assign_variable_to_another_check_bad_var, {
	Variable *var = make_variable("int", U"var", false);

	SkullInt tmp = 0;
	variable_write(var, &tmp);

	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"not_a_variable", &error);
	ASSERT_FALSEY(error);

	Scope *scope = make_scope();
	scope_add_var(scope, var);

	ASSERT_TRUTHY(c32scmp(
		eval_assign(var, node, scope),
		ERR_VAR_NOT_FOUND_(U"not_a_variable")
	));

	free_scope(scope);
})

TEST(eval_assign_string_types_cannot_share_pointers, {
	Variable *var1 = make_variable("str", U"var1", false);
	const char32_t *str1 = NULL;
	variable_write(var1, &str1);

	Variable *var2 = make_variable("str", U"var2", false);
	const char32_t *str2 = c32sdup(U"anything");
	variable_write(var2, &str2);

	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"var2", &error);
	ASSERT_FALSEY(error);

	Scope *scope = make_scope();
	scope_add_var(scope, var1);
	scope_add_var(scope, var2);

	const char32_t *output = eval_assign(var1, node, scope);
	ASSERT_FALSEY(output);

	char32_t *after_var1 = NULL;
	variable_read(&after_var1, var1);
	ASSERT_TRUTHY(c32scmp(after_var1, str2));

	char32_t *after_var2 = NULL;
	variable_read(&after_var2, var2);
	ASSERT_TRUTHY(c32scmp(after_var1, str2));

	ASSERT_NOT_EQUAL(after_var1, after_var2);

	free_scope(scope);
})

TEST(eval_assign_type_type, {
	Variable *var = make_variable("type", U"var", false);

	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"int", &error);
	ASSERT_FALSEY(error);

	Scope *scope = make_scope();
	const char32_t *output = eval_assign(var, node, scope);
	ASSERT_FALSEY(output);

	Type *after = NULL;
	variable_read(&after, var);

	ASSERT_EQUAL(after, &TYPE_INT);

	free_scope(scope);
})

TEST(eval_assign_type_var_cannot_be_type, {
	Variable *var = make_variable("type", U"var", false);

	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"type", &error);
	ASSERT_FALSEY(error);

	Scope *scope = make_scope();

	ASSERT_EQUAL(eval_assign(var, node, scope), ERR_TYPE_TYPE_BAD);

	free_scope(scope);
})

TEST_SELF(eval_assign,
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
	test_eval_assign_type_var_cannot_be_type
)
