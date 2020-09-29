#pragma once

#include "test/testing.h"

#define TEST_EVAL_ASSIGN_BASE(str_type, str_value, real_type, expected_val, expected_error, cmp) \
	const char32_t *error = NULL; \
	AstNode *node = make_ast_tree(str_value, &error); \
	Variable *var = make_variable(str_type, U"x", false); \
	const char32_t *output = eval_assign(var, node, NULL); \
	real_type data = 0; \
	variable_read(&data, var); \
	ASSERT_FALSEY(error); \
	ASSERT_TRUTHY(cmp); \
	ASSERT_TRUTHY(c32scmp( \
		(_Bool)(expected_error) ? (expected_error) : U"", \
		(_Bool)(output) ? (output) : U"" \
	)); \
	free_variable(var)

#define TEST_EVAL_ASSIGN_FLOAT(str_type, str_value, real_type, expected_val, expected_error) \
	TEST_EVAL_ASSIGN_BASE(str_type, str_value, real_type, expected_val, expected_error, (int)data == (int)(expected_val))

#define TEST_EVAL_ASSIGN(str_type, str_value, real_type, expected_val, expected_error) \
	TEST_EVAL_ASSIGN_BASE(str_type, str_value, real_type, expected_val, expected_error, data == (expected_val))

#define TEST_EVAL_ASSIGN_WITH_MATH(data, oper, expected) \
	Scope *scope = make_scope(); \
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
	ASSERT_FALSEY(error); \
	ASSERT_EQUAL(out, (expected)); \
	free_variable(var_a); \
	free_variable(var_b)

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
	ASSERT_FALSEY(error); \
	ASSERT_TRUTHY(c32scmp( \
		(expected), \
		output \
	)); \
	free_variable(var_a); \
	free_variable(var_b)

#define TEST_EVAL_ASSIGN_VAR_MUST_EXIST(oper) \
	Scope *scope = make_scope(); \
	Variable *var_a = make_variable("int", U"a", false); \
	scope_add_var(scope, var_a); \
	const char32_t *error = NULL; \
	AstNode *node = make_ast_tree(U"a " oper " b", &error); \
	Variable *var_b = make_variable("int", U"b", false); \
	const char32_t *output = eval_assign(var_b, node, scope); \
	ASSERT_FALSEY(error); \
	ASSERT_TRUTHY(c32scmp( \
		ERR_VAR_NOT_FOUND_(U"b"), \
		output \
	)); \
	free_variable(var_a)

#define TEST_EVAL_ASSIGN_OPER_AVAILABLE(oper, expected) \
	Scope *scope = make_scope(); \
	Variable *var_a = make_variable("bool", U"a", false); \
	scope_add_var(scope, var_a); \
	const char32_t *error = NULL; \
	AstNode *node = make_ast_tree(U"a " oper U" a", &error); \
	Variable *var_b = make_variable("int", U"b", false); \
	const char32_t *output = eval_assign(var_b, node, scope); \
	ASSERT_FALSEY(error); \
	ASSERT_TRUTHY(c32scmp( \
		(expected), \
		output \
	)); \
	free_variable(var_a)

void eval_assign_test_self(_Bool *);
