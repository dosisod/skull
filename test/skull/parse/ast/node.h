#pragma once

#include "test/testing.h"

#define TEST_AST_TREE(str, type, begin_offset, end_offset) \
	const char32_t *code = str; \
	char *err = NULL; \
	AstNode *node = make_ast_tree(code, &err); \
	if (!node->node_type && !(type)) { \
		return true; \
	} \
	ASSERT_EQUAL(node->node_type, (type)); \
	ASSERT_EQUAL(node->token->begin, code + (begin_offset)); \
	ASSERT_EQUAL(node->token_end->end, code + (end_offset)); \
	ASSERT_FALSEY(node->last); \
	ASSERT_FALSEY(node->next); \
	free_ast_tree(node)

void ast_node_test_self(_Bool *pass);
