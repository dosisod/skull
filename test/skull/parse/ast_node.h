#pragma once

#include "test/testing.h"

#define TEST_AST_TREE(str, node_type, begin_offset, end_offset) \
	const char32_t *code = str; \
	AstNode *node = make_ast_tree(code); \
	if (!node->type && !(node_type)) { \
		return true; \
	} \
	ASSERT_EQUAL(node->type, (node_type)); \
	ASSERT_EQUAL(node->token->begin, code + (begin_offset)); \
	if (node->token_end) { \
		ASSERT_EQUAL(node->token_end->end, code + (end_offset)); \
	} \
	ASSERT_FALSEY(node->last); \
	free_ast_tree(node)

void ast_node_test_self(_Bool *pass);
