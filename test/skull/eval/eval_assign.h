#pragma once

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

TEST_DECL(eval_assign_int)
TEST_DECL(eval_assign_float)
TEST_DECL(eval_assign_bool)
TEST_DECL(eval_assign_rune)
TEST_DECL(eval_assign_rune_escaped)
TEST_DECL(eval_assign_str)
TEST_DECL(eval_assign_add_vars)
TEST_DECL(eval_assign_sub_vars)
TEST_DECL(eval_assign_mult_vars)
TEST_DECL(eval_assign_div_vars)
TEST_DECL(eval_assign_add_vars_types_must_match)
TEST_DECL(eval_assign_sub_vars_types_must_match)
TEST_DECL(eval_assign_mult_vars_types_must_match)
TEST_DECL(eval_assign_div_vars_types_must_match)
TEST_DECL(eval_assign_add_vars_var_must_exist)
TEST_DECL(eval_assign_sub_vars_var_must_exist)
TEST_DECL(eval_assign_mult_vars_var_must_exist)
TEST_DECL(eval_assign_div_vars_var_must_exist)
TEST_DECL(eval_assign_check_lhs_var)
TEST_DECL(eval_assign_add_vars_must_be_addable)
TEST_DECL(eval_assign_sub_vars_must_be_subtractable)
TEST_DECL(eval_assign_mult_vars_must_be_multipliable)
TEST_DECL(eval_assign_div_vars_must_be_divisible)
TEST_DECL(eval_assign_int_overflow)
TEST_DECL(eval_assign_type_mismatch)
TEST_DECL(eval_assign_cannot_assign_non_ints)
TEST_DECL(eval_assign_cannot_assign_non_floats)
TEST_DECL(eval_assign_cannot_assign_non_bools)
TEST_DECL(eval_assign_cannot_assign_non_runes)
TEST_DECL(eval_assign_cannot_assign_non_strs)
TEST_DECL(eval_assign_variable_to_another)
TEST_DECL(eval_assign_variable_to_another_check_same_type)
TEST_DECL(eval_assign_variable_to_another_check_bad_var)
TEST_DECL(eval_assign_string_types_cannot_share_pointers)
TEST_DECL(eval_assign_type_type)
TEST_DECL(eval_assign_type_var_cannot_be_type)

void eval_assign_test_self(_Bool *);
