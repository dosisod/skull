#include <stdbool.h>

#include "../../../src/parse/ast/node.h"
#include "../../../test/testing.h"

TEST(make_ast_node_struct, {
	const wchar_t *code=L"hello";
	token_t *token=tokenize(code);

	ast_node_t node;
	node.node_type=AST_NODE_UNKNOWN;
	node.token=token;
	node.token_end=token;
	node.next=NULL;

	return (
		node.node_type==AST_NODE_UNKNOWN &&
		node.token->begin==token->begin &&
		node.token_end->end==token->end &&
		node.next==NULL
	);
});

TEST(make_ast_node, {
	ast_node_t *node=make_ast_node();

	const bool pass=(
		node->node_type==AST_NODE_UNKNOWN &&
		node->token==NULL &&
		node->token_end==NULL &&
		node->next==NULL
	);

	free(node);

	return pass;
});

TEST(ast_token_cmp, {
	const wchar_t *code=L"x: int = 0";
	token_t *token=tokenize(code);
	make_default_types();
	classify_tokens(token);

	const bool pass=ast_token_cmp(
		token,
		TOKEN_NEW_IDENTIFIER,
		TOKEN_TYPE,
		TOKEN_OPER_EQUAL,
		TOKEN_INT_CONST,
		-1
	)==token->next->next->next;

	free_types();
	free_tokens(token);

	return pass;
});

TEST(ast_token_cmp_extra_tokens, {
	const wchar_t *code=L"x: int = 0 extra";
	token_t *token=tokenize(code);
	make_default_types();
	classify_tokens(token);

	const bool pass=ast_token_cmp(
		token,
		TOKEN_NEW_IDENTIFIER,
		TOKEN_TYPE,
		TOKEN_OPER_EQUAL,
		TOKEN_INT_CONST,
		-1
	)==token->next->next->next;

	free_types();
	free_tokens(token);

	return pass;
});

TEST(ast_token_cmp_missing_tokens, {
	const wchar_t *code=L"x: int = 0";
	token_t *token=tokenize(code);
	make_default_types();
	classify_tokens(token);

	const bool pass=ast_token_cmp(
		token,
		TOKEN_IDENTIFIER,
		TOKEN_TYPE,
		TOKEN_OPER_EQUAL,
		TOKEN_INT_CONST,
		TOKEN_UNKNOWN, // missing an extra "unknown" token
		-1
	)==token;

	free_types();
	free_tokens(token);

	return pass;
});

TEST(push_ast_node, {
	const wchar_t *code=L"x: int = 0";
	token_t *token=tokenize(code);
	token_t *last=token;

	make_default_types();
	classify_tokens(token);

	ast_node_t *node=make_ast_node();
	ast_node_t *tmp=node;

	token=ast_token_cmp(
		token,
		TOKEN_NEW_IDENTIFIER,
		TOKEN_TYPE,
		TOKEN_OPER_EQUAL,
		TOKEN_INT_CONST,
		-1
	);
	push_ast_node(token, &last, AST_NODE_VAR_DEF, &node);

	const bool pass=(tmp->next==node);

	free_types();
	free_tokens(token);

	return pass;
});

#define TEST_AST_TREE(str, type, begin_offset, end_offset) \
	const wchar_t *code=str; \
	make_default_types(); \
	ast_node_t *node=make_ast_tree(code); \
	const bool pass=( \
		node->node_type==type && \
		node->token->begin==(code + begin_offset) && \
		node->token_end->end==(code + end_offset) && \
		node->next!=NULL \
	); \
	free_types(); \
	free(node); \
	return pass;

TEST(make_ast_tree_variable_def, {
	TEST_AST_TREE(L"x: int =", AST_NODE_VAR_DEF, 0, 8);
});

TEST(make_ast_tree_mutable_variable_def, {
	TEST_AST_TREE(L"mut x: int =", AST_NODE_MUT_VAR_DEF, 0, 12);
});

TEST(make_ast_tree_auto_variable_def, {
	TEST_AST_TREE(L"x :=", AST_NODE_AUTO_VAR_DEF, 0, 4);
});

TEST(make_ast_tree_auto_mutable_variable_def, {
	TEST_AST_TREE(L"mut x :=", AST_NODE_MUT_AUTO_VAR_DEF, 0, 8);
});

TEST(make_ast_tree_many_lines, {
	const wchar_t *code=L"x: int = 0\ny: int = 0";
	make_default_types();
	ast_node_t *node=make_ast_tree(code);

	const bool pass=(
		node->node_type==AST_NODE_VAR_DEF &&
		node->token->begin==code &&
		node->token_end->end==(code + 8) &&
		node->next!=NULL &&
		node->next->node_type==AST_NODE_INT_CONST &&
		node->next->token->begin==(code + 9) &&
		node->next->token_end->end==(code + 10) &&
		node->next->next!=NULL &&
		node->next->next->node_type==AST_NODE_VAR_DEF &&
		node->next->next->token->begin==(code + 11) &&
		node->next->next->token_end->end==(code + 19) &&
		node->next->next->next!=NULL &&
		node->next->next->next->node_type==AST_NODE_INT_CONST &&
		node->next->next->next->token->begin==(code + 20) &&
		node->next->next->next->token_end->end==(code + 21)
	);

	free_types();
	free(node->next);
	free(node);

	return pass;
});

TEST(make_ast_tree_with_whitespace, {
	ast_node_t *node=make_ast_tree(L"");

	const bool pass=(
		node!=NULL &&
		node->next==NULL &&
		node->token==NULL
	);

	free(node);
	return pass;
});

TEST(make_ast_tree_var_assign, {
	TEST_AST_TREE(L"x =", AST_NODE_VAR_ASSIGN, 0, 3);
});

TEST(make_ast_tree_var_add, {
	TEST_AST_TREE(L"x + y", AST_NODE_ADD_VAR, 0, 5);
});

TEST(make_ast_tree_return, {
	TEST_AST_TREE(L"return 0", AST_NODE_RETURN, 0, 8);
});

TEST(make_ast_tree_no_param_func, {
	TEST_AST_TREE(L"func[]", AST_NODE_NO_PARAM_FUNC, 0, 6);
});

TEST(make_ast_tree_int_const, {
	TEST_AST_TREE(L"1234", AST_NODE_INT_CONST, 0, 4);
});

TEST(make_ast_tree_float_const, {
	TEST_AST_TREE(L"3.1415", AST_NODE_FLOAT_CONST, 0, 6);
});

TEST(make_ast_tree_bool_const_true, {
	TEST_AST_TREE(L"true", AST_NODE_BOOL_CONST, 0, 4);
});

TEST(make_ast_tree_bool_const_false, {
	TEST_AST_TREE(L"false", AST_NODE_BOOL_CONST, 0, 5);
});

TEST(make_ast_tree_char_const, {
	TEST_AST_TREE(L"'x'", AST_NODE_CHAR_CONST, 1, 2);
});

TEST(make_ast_tree_str_const, {
	TEST_AST_TREE(L"\"abc\"", AST_NODE_STR_CONST, 1, 4);
});

#undef TEST_AST_TREE

/* Once a certain bug is fixed, this test can run
bool test_make_ast_tree_colon_suffix_required(void) {
	const wchar_t *code=L"x int = 0";
	ast_node_t *node=make_ast_tree(code);

	const bool pass=(
		node->node_type==AST_NODE_UNKNOWN &&
		node->token->begin==code &&
		node->token_end->end==(code + 9) &&
		node->next!=NULL
	);

	free(node);
	return pass;
}
*/

TEST(free_ast_tree, {
	ast_node_t *node=make_ast_tree(L"hello world");

	free_ast_tree(node);
	return true;
});

void ast_node_test_self(bool *pass) {
	tests_t tests={
		test_make_ast_node_struct,
		test_make_ast_node,
		test_ast_token_cmp,
		test_ast_token_cmp_extra_tokens,
		test_ast_token_cmp_missing_tokens,
		test_push_ast_node,
		test_make_ast_tree_variable_def,
		test_make_ast_tree_mutable_variable_def,
		test_make_ast_tree_auto_variable_def,
		test_make_ast_tree_auto_mutable_variable_def,
		test_make_ast_tree_many_lines,
		test_make_ast_tree_with_whitespace,
		test_make_ast_tree_var_assign,
		test_make_ast_tree_var_add,
		test_make_ast_tree_return,
		test_make_ast_tree_no_param_func,
		test_make_ast_tree_int_const,
		test_make_ast_tree_float_const,
		test_make_ast_tree_bool_const_true,
		test_make_ast_tree_bool_const_false,
		test_make_ast_tree_char_const,
		test_make_ast_tree_str_const,
		//test_make_ast_tree_colon_suffix_required,
		test_free_ast_tree,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
