#pragma once

#include "test/testing.h"

#define TEST_AST_TREE(str, type, begin_offset, end_offset) \
	const char32_t *code = str; \
	const char32_t *err = NULL; \
	AstNode *node = make_ast_tree(code, &err); \
	if (!node->node_type && !(type)) { \
		return true; \
	} \
	ASSERT_EQUAL(node->node_type, (type)); \
	ASSERT_EQUAL(node->token->begin, code + (begin_offset)); \
	ASSERT_EQUAL(node->token_end->end, code + (end_offset)); \
	ASSERT_FALSEY(node->last); \
	ASSERT_FALSEY(node->next); \
	free(node)

TEST_DECL(make_ast_node_struct)
TEST_DECL(make_ast_node)
TEST_DECL(ast_token_cmp)
TEST_DECL(ast_token_cmp_extra_tokens)
TEST_DECL(ast_token_cmp_missing_tokens)
TEST_DECL(ast_token_cmp_any_token)
TEST_DECL(ast_token_cmp_optional_combo)
TEST_DECL(push_ast_node)
TEST_DECL(make_ast_tree_identifier)
TEST_DECL(make_ast_tree_variable_def)
TEST_DECL(make_ast_tree_mutable_variable_def)
TEST_DECL(make_ast_tree_auto_variable_def)
TEST_DECL(make_ast_tree_auto_mutable_variable_def)
TEST_DECL(make_ast_tree_with_nothing)
TEST_DECL(make_ast_tree_var_assign)
TEST_DECL(make_ast_tree_import)
TEST_DECL(make_ast_tree_var_add)
TEST_DECL(make_ast_tree_var_sub)
TEST_DECL(make_ast_tree_var_mult)
TEST_DECL(make_ast_tree_var_div)
TEST_DECL(make_ast_tree_return)
TEST_DECL(make_ast_tree_return_var)
TEST_DECL(make_ast_tree_if)
TEST_DECL(make_ast_tree_int_const)
TEST_DECL(make_ast_tree_float_const)
TEST_DECL(make_ast_tree_bool_const_true)
TEST_DECL(make_ast_tree_bool_const_false)
TEST_DECL(make_ast_tree_rune_const)
TEST_DECL(make_ast_tree_str_const)
TEST_DECL(make_ast_tree_type_const)
TEST_DECL(make_ast_tree_comment)
TEST_DECL(make_ast_tree_bad_recursive_combo_fails)
TEST_DECL(free_ast_tree)

void ast_node_test_self(_Bool *pass);
