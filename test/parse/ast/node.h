#include <stdbool.h>

#include "skull/parse/ast/node.h"
#include "skull/parse/classify.h"

#include "test/testing.h"

TEST(make_ast_node_struct, {
	const char32_t *code=U"hello";
	token_t *token=tokenize(code);

	ast_node_t node;
	node.node_type=AST_NODE_UNKNOWN;
	node.token=token;
	node.token_end=token;
	node.next=NULL;
	node.last=NULL;

	return (
		node.node_type==AST_NODE_UNKNOWN &&
		node.token->begin==token->begin &&
		node.token_end->end==token->end &&
		!node.last &&
		!node.next
	);
})

TEST(make_ast_node, {
	ast_node_t *node=make_ast_node();

	const bool pass=(
		node->node_type==AST_NODE_UNKNOWN &&
		!node->token &&
		!node->token_end &&
		!node->last &&
		!node->next
	);

	free(node);
	return pass;
})

TEST(ast_token_cmp, {
	const char32_t *code=U"x: int = 0";
	token_t *token=tokenize(code);
	classify_tokens(token);

	const bool pass=(ast_token_cmp(
		token, (int[]){
			TOKEN_NEW_IDENTIFIER,
			TOKEN_TYPE,
			TOKEN_OPER_EQUAL,
			TOKEN_INT_CONST, -1
		}
	)==token->next->next->next);

	free_tokens(token);
	return pass;
})

TEST(ast_token_cmp_extra_tokens, {
	const char32_t *code=U"x: int = 0 extra";
	token_t *token=tokenize(code);
	classify_tokens(token);

	const bool pass=(ast_token_cmp(
		token, (int[]) {
			TOKEN_NEW_IDENTIFIER,
			TOKEN_TYPE,
			TOKEN_OPER_EQUAL,
			TOKEN_INT_CONST, -1
		}
	)==token->next->next->next);

	free_tokens(token);
	return pass;
})

TEST(ast_token_cmp_missing_tokens, {
	const char32_t *code=U"x: int = 0";
	token_t *token=tokenize(code);
	classify_tokens(token);

	const bool pass=(ast_token_cmp(
		token, (int[]) {
			TOKEN_IDENTIFIER,
			TOKEN_TYPE,
			TOKEN_OPER_EQUAL,
			TOKEN_INT_CONST,
			TOKEN_UNKNOWN, // missing an extra "unknown" token
			-1
		}
	)==token);

	free_tokens(token);
	return pass;
})

TEST(ast_token_cmp_any_token, {
	const char32_t *code=U"[anything]";
	token_t *token=tokenize(code);
	classify_tokens(token);

	const bool pass=(ast_token_cmp(
		token, (int[]) {
			TOKEN_BRACKET_OPEN,
			TOKEN_ANY_NON_BRACKET_TOKEN,
			TOKEN_BRACKET_CLOSE, -1
		}
	)==token->next->next);

	free_tokens(token);
	return pass;
})

TEST(push_ast_node, {
	const char32_t *code=U"x: int = 0";
	token_t *token=tokenize(code);
	token_t *last=token;

	classify_tokens(token);

	ast_node_t *node=make_ast_node();
	ast_node_t *tmp=node;

	token=ast_token_cmp(token, (int[]) {
		TOKEN_NEW_IDENTIFIER,
		TOKEN_TYPE,
		TOKEN_OPER_EQUAL,
		TOKEN_INT_CONST, -1
	});
	push_ast_node(token, &last, AST_NODE_VAR_DEF, &node);

	const bool pass=(
		!tmp->last &&
		tmp->next==node &&
		tmp->next->last==tmp
	);

	free_tokens(token);
	return pass;
})

#define TEST_AST_TREE(str, type, begin_offset, end_offset) \
	const char32_t *code=str; \
	ast_node_t *node=make_ast_tree(code); \
	const bool pass=( \
		node->node_type==(type) && \
		node->token->begin==(code + (begin_offset)) && \
		node->token_end->end==(code + (end_offset)) && \
		!node->last && \
		!node->next \
	); \
	free(node); \
	return pass;

TEST(make_ast_tree_identifier, {
	TEST_AST_TREE(U"x", AST_NODE_IDENTIFIER, 0, 1);
})

TEST(make_ast_tree_variable_def, {
	TEST_AST_TREE(U"x: int =", AST_NODE_VAR_DEF, 0, 8);
})

TEST(make_ast_tree_mutable_variable_def, {
	TEST_AST_TREE(U"mut x: int =", AST_NODE_MUT_VAR_DEF, 0, 12);
})

TEST(make_ast_tree_auto_variable_def, {
	TEST_AST_TREE(U"x :=", AST_NODE_AUTO_VAR_DEF, 0, 4);
})

TEST(make_ast_tree_auto_mutable_variable_def, {
	TEST_AST_TREE(U"mut x :=", AST_NODE_MUT_AUTO_VAR_DEF, 0, 8);
})

TEST(make_ast_tree_with_whitespace, {
	ast_node_t *node=make_ast_tree(U"");

	const bool pass=(
		node &&
		!node->next &&
		!node->token
	);

	free(node);
	return pass;
})

TEST(make_ast_tree_var_assign, {
	TEST_AST_TREE(U"x =", AST_NODE_VAR_ASSIGN, 0, 3);
})

TEST(make_ast_tree_var_add, {
	TEST_AST_TREE(U"x + y", AST_NODE_ADD_VAR, 0, 5);
})

TEST(make_ast_tree_var_sub, {
	TEST_AST_TREE(U"x - y", AST_NODE_SUB_VAR, 0, 5);
})

TEST(make_ast_tree_var_mult, {
	TEST_AST_TREE(U"x * y", AST_NODE_MULT_VAR, 0, 5);
})

TEST(make_ast_tree_var_div, {
	TEST_AST_TREE(U"x / y", AST_NODE_DIV_VAR, 0, 5);
})

TEST(make_ast_tree_return, {
	TEST_AST_TREE(U"return 0", AST_NODE_RETURN, 0, 8);
})

TEST(make_ast_tree_return_var, {
	TEST_AST_TREE(U"return x", AST_NODE_RETURN, 0, 8);
})

TEST(make_ast_tree_no_param_func, {
	TEST_AST_TREE(U"func[]", AST_NODE_NO_PARAM_FUNC, 0, 6);
})

TEST(make_ast_tree_one_param_func, {
	TEST_AST_TREE(U"func[abc]", AST_NODE_ONE_PARAM_FUNC, 0, 9);
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

TEST(make_ast_tree_char_const, {
	TEST_AST_TREE(U"'x'", AST_NODE_CHAR_CONST, 1, 2);
})

TEST(make_ast_tree_str_const, {
	TEST_AST_TREE(U"\"abc\"", AST_NODE_STR_CONST, 1, 4);
})

TEST(make_ast_tree_type_const, {
	TEST_AST_TREE(U"int", AST_NODE_TYPE_CONST, 0, 3);
})

TEST(make_ast_tree_comment, {
	TEST_AST_TREE(U"# this is a comment", AST_NODE_COMMENT, 0, 19);
})

#undef TEST_AST_TREE

TEST(free_ast_tree, {
	ast_node_t *node=make_ast_tree(U"hello world");

	free_ast_tree(node);
	return true;
})

void ast_node_test_self(bool *pass) {
	tests_t tests={
		test_make_ast_node_struct,
		test_make_ast_node,
		test_ast_token_cmp,
		test_ast_token_cmp_extra_tokens,
		test_ast_token_cmp_missing_tokens,
		test_ast_token_cmp_any_token,
		test_push_ast_node,
		test_make_ast_tree_identifier,
		test_make_ast_tree_variable_def,
		test_make_ast_tree_mutable_variable_def,
		test_make_ast_tree_auto_variable_def,
		test_make_ast_tree_auto_mutable_variable_def,
		test_make_ast_tree_with_whitespace,
		test_make_ast_tree_var_assign,
		test_make_ast_tree_var_add,
		test_make_ast_tree_var_sub,
		test_make_ast_tree_var_mult,
		test_make_ast_tree_var_div,
		test_make_ast_tree_return,
		test_make_ast_tree_return_var,
		test_make_ast_tree_no_param_func,
		test_make_ast_tree_one_param_func,
		test_make_ast_tree_int_const,
		test_make_ast_tree_float_const,
		test_make_ast_tree_bool_const_true,
		test_make_ast_tree_bool_const_false,
		test_make_ast_tree_char_const,
		test_make_ast_tree_str_const,
		test_make_ast_tree_type_const,
		test_make_ast_tree_comment,
		test_free_ast_tree,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
