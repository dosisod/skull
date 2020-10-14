#include <stdbool.h>

#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/parse/ast/node.h"
#include "skull/parse/classify.h"

#include "test/skull/parse/ast/node.h"
#include "test/testing.h"

TEST(make_ast_node, {
	AstNode *node = make_ast_node();

	ASSERT_EQUAL(node->node_type, AST_NODE_UNKNOWN);
	ASSERT_FALSEY(node->token);
	ASSERT_FALSEY(node->token_end);
	ASSERT_FALSEY(node->last);
	ASSERT_FALSEY(node->next);

	free_ast_tree(node);
})

TEST(push_ast_node, {
	Token *token = tokenize(U"x: int = 0");
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

TEST(make_ast_tree_identifier, {
	TEST_AST_TREE(U"x", AST_NODE_IDENTIFIER, 0, 1);
})

TEST(make_ast_tree_variable_def, {
	TEST_AST_TREE(U"x: int =", AST_NODE_VAR_DEF, 0, 8);
})

TEST(make_ast_tree_mutable_variable_def, {
	TEST_AST_TREE(U"mut x: int =", AST_NODE_VAR_DEF, 0, 12);
})

TEST(make_ast_tree_auto_variable_def, {
	TEST_AST_TREE(U"x :=", AST_NODE_VAR_DEF, 0, 4);
})

TEST(make_ast_tree_auto_mutable_variable_def, {
	TEST_AST_TREE(U"mut x :=", AST_NODE_VAR_DEF, 0, 8);
})

TEST(make_ast_tree_with_nothing, {
	char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"", &error);

	ASSERT_TRUTHY(c32scmp(ERR_UNEXPECTED_TOKEN_(U""), error));
	ASSERT_FALSEY(node);

	free(error);
	free_ast_tree(node);
})

TEST(make_ast_tree_var_assign, {
	TEST_AST_TREE(U"x =", AST_NODE_VAR_ASSIGN, 0, 3);
})

TEST(make_ast_tree_import, {
	TEST_AST_TREE(U"import x from \"y\"", AST_NODE_IMPORT, 0, 17);
})

TEST(make_ast_tree_return, {
	TEST_AST_TREE(U"return 0", AST_NODE_RETURN, 0, 8);
})

TEST(make_ast_tree_return_var, {
	TEST_AST_TREE(U"return x", AST_NODE_RETURN, 0, 8);
})

TEST(make_ast_tree_if, {
	const char32_t *code = U"if true [ return 1 ]";
	char32_t *error = NULL;
	AstNode *node = make_ast_tree(code, &error);

	ASSERT_FALSEY(error);
	ASSERT_EQUAL(node->node_type, AST_NODE_IF);
	ASSERT_EQUAL(node->token->begin, code);
	ASSERT_EQUAL(node->token_end->end, code + 7);
	ASSERT_FALSEY(node->last);
	ASSERT_TRUTHY(node->child);
	ASSERT_EQUAL(node->child->token->begin, code + 10);
	ASSERT_EQUAL(node->child->token_end->end, code + 18);
	ASSERT_FALSEY(node->next);

	free_ast_tree(node);
})

TEST(make_ast_tree_if_with_var, {
	const char32_t *code = U"if x [ return 1 ]";
	char32_t *error = NULL;
	AstNode *node = make_ast_tree(code, &error);

	ASSERT_FALSEY(error);
	ASSERT_EQUAL(node->node_type, AST_NODE_IF);
	ASSERT_EQUAL(node->token->begin, code);
	ASSERT_EQUAL(node->token_end->end, code + 4);
	ASSERT_FALSEY(node->last);
	ASSERT_TRUTHY(node->child);
	ASSERT_EQUAL(node->child->token->begin, code + 7);
	ASSERT_EQUAL(node->child->token_end->end, code + 15);
	ASSERT_FALSEY(node->next);

	free_ast_tree(node);
})

TEST(make_ast_tree_int_const, {
	TEST_AST_TREE(U"1234", AST_NODE_INT_CONST, 0, 4);
})

TEST(make_ast_tree_float_const, {
	TEST_AST_TREE(U"3.1415", AST_NODE_FLOAT_CONST, 0, 6);
})

TEST(make_ast_tree_bool_const_true, {
	TEST_AST_TREE(U"true", AST_NODE_BOOL_CONST, 0, 4);
})

TEST(make_ast_tree_bool_const_false, {
	TEST_AST_TREE(U"false", AST_NODE_BOOL_CONST, 0, 5);
})

TEST(make_ast_tree_rune_const, {
	TEST_AST_TREE(U"'x'", AST_NODE_RUNE_CONST, 0, 3);
})

TEST(make_ast_tree_str_const, {
	TEST_AST_TREE(U"\"abc\"", AST_NODE_STR_CONST, 0, 5);
})

TEST(make_ast_tree_type_const, {
	TEST_AST_TREE(U"int", AST_NODE_TYPE_CONST, 0, 3);
})

TEST(make_ast_tree_comment, {
	TEST_AST_TREE(U"# this is a comment", AST_NODE_COMMENT, 0, 19);
})

TEST(make_ast_tree_recursive_check_fails, {
	char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"[", &error);

	ASSERT_TRUTHY(c32scmp(ERR_EOF_NO_BRACKET, error));
	ASSERT_FALSEY(node);

	free_ast_tree(node);
	free(error);
})

TEST(free_ast_tree, {
	char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"hello world", &error);

	free_ast_tree(node);
})

TEST_SELF(ast_node,
	test_make_ast_node,
	test_push_ast_node,
	test_make_ast_tree_identifier,
	test_make_ast_tree_variable_def,
	test_make_ast_tree_mutable_variable_def,
	test_make_ast_tree_auto_variable_def,
	test_make_ast_tree_auto_mutable_variable_def,
	test_make_ast_tree_with_nothing,
	test_make_ast_tree_var_assign,
	test_make_ast_tree_import,
	test_make_ast_tree_return,
	test_make_ast_tree_return_var,
	test_make_ast_tree_if,
	test_make_ast_tree_if_with_var,
	test_make_ast_tree_int_const,
	test_make_ast_tree_float_const,
	test_make_ast_tree_bool_const_true,
	test_make_ast_tree_bool_const_false,
	test_make_ast_tree_rune_const,
	test_make_ast_tree_str_const,
	test_make_ast_tree_type_const,
	test_make_ast_tree_comment,
	test_make_ast_tree_recursive_check_fails,
	test_free_ast_tree
)
