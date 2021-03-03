#include <stdbool.h>

#include "skull/common/str.h"
#include "skull/parse/ast_node.h"
#include "skull/parse/classify.h"

#include "test/skull/parse/ast_node.h"
#include "test/testing.h"

TEST(make_ast_node, {
	AstNode *node = make_ast_node();

	ASSERT_EQUAL(node->type, AST_NODE_UNKNOWN);
	ASSERT_FALSEY(node->token);
	ASSERT_FALSEY(node->token_end);
	ASSERT_FALSEY(node->last);
	ASSERT_FALSEY(node->next);

	free_ast_tree(node);
})

TEST(push_ast_node, {
	Token *token = tokenize(U"x: Int = 0");
	Token *last = token;

	classify_tokens(token);

	AstNode *node = make_ast_node();
	AstNode *tmp = node;

	push_ast_node(token, &last, AST_NODE_VAR_DEF, &node);

	ASSERT_FALSEY(tmp->last);
	ASSERT_EQUAL(tmp->next, node);
	ASSERT_EQUAL(tmp->next->last, tmp);

	free_ast_tree(tmp);
})

TEST(make_ast_tree_variable_def, {
	TEST_AST_TREE(U"x: Int = 0", AST_NODE_VAR_DEF, 0, 8);
})

TEST(make_ast_tree_mutable_variable_def, {
	TEST_AST_TREE(U"mut x: Int = 0", AST_NODE_VAR_DEF, 0, 12);
})

TEST(make_ast_tree_auto_variable_def, {
	TEST_AST_TREE(U"x := 0", AST_NODE_VAR_DEF, 0, 4);
})

TEST(make_ast_tree_auto_mutable_variable_def, {
	TEST_AST_TREE(U"mut x := 0", AST_NODE_VAR_DEF, 0, 8);
})

TEST(make_ast_tree_var_assign, {
	TEST_AST_TREE(U"x = 0", AST_NODE_VAR_ASSIGN, 0, 3);
})

TEST(make_ast_tree_external, {
	TEST_AST_TREE(U"external x()\n", AST_NODE_FUNCTION_PROTO, 0, 12);
})

TEST(make_ast_tree_function, {
	TEST_AST_TREE(U"x()", AST_NODE_FUNCTION, 0, 1);
})

TEST(make_ast_tree_return, {
	TEST_AST_TREE(U"return 0", AST_NODE_RETURN, 0, 6);
})
TEST(make_ast_tree_if, {
	const char32_t *const code = U"if true { }";
	AstNode *node = make_ast_tree(code);

	ASSERT_EQUAL(node->type, AST_NODE_IF);
	ASSERT_EQUAL(node->token->begin, code);
	ASSERT_EQUAL(node->token_end->end, code + 2);
	ASSERT_FALSEY(node->last);
	ASSERT_TRUTHY(node->next);
	ASSERT_TRUTHY(node->next->child);

	free_ast_tree(node);
})

TEST(make_ast_tree_if_with_var, {
	const char32_t *const code = U"if x { }";
	AstNode *node = make_ast_tree(code);

	ASSERT_EQUAL(node->type, AST_NODE_IF);
	ASSERT_EQUAL(node->token->begin, code);
	ASSERT_EQUAL(node->token_end->end, code + 2);
	ASSERT_FALSEY(node->last);
	ASSERT_TRUTHY(node->next);
	ASSERT_TRUTHY(node->next->child);

	free_ast_tree(node);
})

TEST(make_ast_tree_comment, {
	TEST_AST_TREE(U"# this is a comment", AST_NODE_COMMENT, 0, 19);
})

TEST(free_ast_tree, {
	free_ast_tree(make_ast_tree(U"# whatever"));
})

TEST_SELF(ast_node,
	test_make_ast_node,
	test_push_ast_node,
	test_make_ast_tree_variable_def,
	test_make_ast_tree_mutable_variable_def,
	test_make_ast_tree_auto_variable_def,
	test_make_ast_tree_auto_mutable_variable_def,
	test_make_ast_tree_var_assign,
	test_make_ast_tree_external,
	test_make_ast_tree_function,
	test_make_ast_tree_return,
	test_make_ast_tree_if,
	test_make_ast_tree_if_with_var,
	test_make_ast_tree_comment,
	test_free_ast_tree
)
