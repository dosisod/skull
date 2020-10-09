#include <stdbool.h>

#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/parse/ast/node.h"
#include "skull/parse/classify.h"

#include "test/skull/parse/ast/node.h"
#include "test/testing.h"

TEST(make_ast_node_struct, {
	Token *token = tokenize(U"hello");

	AstNode node;
	node.node_type = AST_NODE_UNKNOWN;
	node.token = token;
	node.token_end = token;
	node.next = NULL;
	node.last = NULL;

	ASSERT_EQUAL(node.node_type, AST_NODE_UNKNOWN);
	ASSERT_EQUAL(node.token->begin, token->begin);
	ASSERT_EQUAL(node.token_end->end, token->end);
	ASSERT_FALSEY(node.last);
	ASSERT_FALSEY(node.next);
})

TEST(make_ast_node, {
	AstNode *node = make_ast_node();

	ASSERT_EQUAL(node->node_type, AST_NODE_UNKNOWN);
	ASSERT_FALSEY(node->token);
	ASSERT_FALSEY(node->token_end);
	ASSERT_FALSEY(node->last);
	ASSERT_FALSEY(node->next);

	free(node);
})

TEST(ast_token_cmp, {
	Token *token = tokenize(U"x: int = 0");
	classify_tokens(token);

	bool ast_pass = false;
	const Token *out = ast_token_cmp(
		token, (Combo[]){
			{ .tok = TOKEN_NEW_IDENTIFIER },
			{ .tok = TOKEN_TYPE },
			{ .tok = TOKEN_OPER_EQUAL },
			{ .tok = TOKEN_INT_CONST },
			{0}
		},
		&ast_pass
	);

	ASSERT_TRUTHY(ast_pass);
	ASSERT_EQUAL(token->next->next->next, out);

	free_tokens(token);
})

TEST(ast_token_cmp_extra_tokens, {
	Token *token = tokenize(U"x: int = 0 extra");
	classify_tokens(token);

	bool ast_pass = false;
	const Token *out = ast_token_cmp(
		token, (Combo[]) {
			{ .tok = TOKEN_NEW_IDENTIFIER },
			{ .tok = TOKEN_TYPE },
			{ .tok = TOKEN_OPER_EQUAL },
			{ .tok = TOKEN_INT_CONST },
			{0}
		},
		&ast_pass
	);

	ASSERT_TRUTHY(ast_pass);
	ASSERT_EQUAL(token->next->next->next, out);

	free_tokens(token);
})

TEST(ast_token_cmp_missing_tokens, {
	Token *token = tokenize(U"x: int = 0");
	classify_tokens(token);

	bool ast_pass = false;
	const Token *out = ast_token_cmp(
		token, (Combo[]) {
			{ .tok = TOKEN_IDENTIFIER },
			{ .tok = TOKEN_TYPE },
			{ .tok = TOKEN_OPER_EQUAL },
			{ .tok = TOKEN_INT_CONST },
			{ .tok = TOKEN_UNKNOWN }, // missing an extra "unknown" token
			{0}
		},
		&ast_pass
	);

	ASSERT_FALSEY(ast_pass);
	ASSERT_EQUAL(token, out);

	free_tokens(token);
})

TEST(ast_token_cmp_any_token, {
	Token *token = tokenize(U"[anything]");
	classify_tokens(token);

	bool ast_pass = false;
	const Token *out = ast_token_cmp(
		token, (Combo[]) {
			{ .tok = TOKEN_BRACKET_OPEN },
			{ .tok = TOKEN_ANY_NON_BRACKET_TOKEN },
			{ .tok = TOKEN_BRACKET_CLOSE },
			{0}
		},
		&ast_pass
	);

	ASSERT_TRUTHY(ast_pass);
	ASSERT_EQUAL(token->next->next, out);

	free_tokens(token);
})

static Combo optional_combo[] = {
	{ .tok = TOKEN_BRACKET_OPEN },
	{ .tok = TOKEN_IDENTIFIER, .rule = RULE_OPTIONAL },
	{ .tok = TOKEN_BRACKET_CLOSE },
	{0}
};

TEST(ast_token_cmp_optional_combo, {
	Token *token_with = tokenize(U"[ x ]");
	classify_tokens(token_with);

	Token *token_without = tokenize(U"[ ]");
	classify_tokens(token_without);

	bool ast_pass_with = false;
	const Token *out_with = ast_token_cmp(token_with, optional_combo, &ast_pass_with);

	bool ast_pass_without = false;
	const Token *out_without = ast_token_cmp(token_without, optional_combo, &ast_pass_without);

	ASSERT_TRUTHY(ast_pass_with);
	ASSERT_EQUAL(token_with->next->next, out_with);
	ASSERT_TRUTHY(ast_pass_without);
	ASSERT_EQUAL(token_without->next, out_without);

	free_tokens(token_with);
	free_tokens(token_without);
})

TEST(push_ast_node, {
	Token *token = tokenize(U"x: int = 0");
	Token *last = token;

	classify_tokens(token);

	AstNode *node = make_ast_node();
	AstNode *tmp = node;

	bool ast_pass = false;
	token = ast_token_cmp(token, (Combo[]) {
		{ .tok = TOKEN_NEW_IDENTIFIER },
		{ .tok = TOKEN_TYPE },
		{ .tok = TOKEN_OPER_EQUAL },
		{ .tok = TOKEN_INT_CONST },
		{0}
	}, &ast_pass);

	push_ast_node(token, &last, AST_NODE_VAR_DEF, &node);

	ASSERT_TRUTHY(ast_pass);
	ASSERT_FALSEY(tmp->last);
	ASSERT_EQUAL(tmp->next, node);
	ASSERT_EQUAL(tmp->next->last, tmp);

	free_tokens(token);
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
	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"", &error);

	ASSERT_TRUTHY(error);
	ASSERT_TRUTHY(c32scmp(ERR_UNEXPECTED_TOKEN_(U""), error));
	ASSERT_FALSEY(node);

	free(node);
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
	const char32_t *error = NULL;
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

	free(node);
})

TEST(make_ast_tree_if_with_var, {
	const char32_t *code = U"if x [ return 1 ]";
	const char32_t *error = NULL;
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

	free(node);
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

TEST(make_ast_tree_bad_recursive_combo_fails, {
	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"[", &error);

	ASSERT_TRUTHY(c32scmp(ERR_EOF_NO_BRACKET, error));
	ASSERT_FALSEY(node);
})

TEST(free_ast_tree, {
	const char32_t *error = NULL;
	AstNode *node = make_ast_tree(U"hello world", &error);

	free_ast_tree(node);
})

TEST_SELF(ast_node,
	test_make_ast_node_struct,
	test_make_ast_node,
	test_ast_token_cmp,
	test_ast_token_cmp_extra_tokens,
	test_ast_token_cmp_missing_tokens,
	test_ast_token_cmp_any_token,
	test_ast_token_cmp_optional_combo,
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
	test_make_ast_tree_bad_recursive_combo_fails,
	test_free_ast_tree
)
