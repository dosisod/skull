#include <stdbool.h>

#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/parse/ast/node.h"
#include "skull/parse/classify.h"

#include "test/skull/parse/ast/node.h"
#include "test/testing.h"

TEST(make_ast_node_struct, {
	const char32_t *code=U"hello";
	Token *token=tokenize(code);

	AstNode node;
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
	AstNode *node=make_ast_node();

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
	Token *token=tokenize(code);
	classify_tokens(token);

	bool ast_pass=false;
	const Token *out=ast_token_cmp(
		token, (Combo[]){
			{ .tok = TOKEN_NEW_IDENTIFIER },
			{ .tok = TOKEN_TYPE },
			{ .tok = TOKEN_OPER_EQUAL },
			{ .tok = TOKEN_INT_CONST },
			{0}
		},
		&ast_pass
	);

	const bool pass=(
		ast_pass &&
		token->next->next->next == out
	);

	free_tokens(token);
	return pass;
})

TEST(ast_token_cmp_extra_tokens, {
	const char32_t *code=U"x: int = 0 extra";
	Token *token=tokenize(code);
	classify_tokens(token);

	bool ast_pass=false;
	const Token *out=ast_token_cmp(
		token, (Combo[]) {
			{ .tok = TOKEN_NEW_IDENTIFIER },
			{ .tok = TOKEN_TYPE },
			{ .tok = TOKEN_OPER_EQUAL },
			{ .tok = TOKEN_INT_CONST },
			{0}
		},
		&ast_pass
	);

	const bool pass=(
		ast_pass &&
		token->next->next->next == out
	);

	free_tokens(token);
	return pass;
})

TEST(ast_token_cmp_missing_tokens, {
	const char32_t *code=U"x: int = 0";
	Token *token=tokenize(code);
	classify_tokens(token);

	bool ast_pass=false;
	const Token *out=ast_token_cmp(
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
	const bool pass=(
		!ast_pass &&
		token == out
	);

	free_tokens(token);
	return pass;
})

TEST(ast_token_cmp_any_token, {
	const char32_t *code=U"[anything]";
	Token *token=tokenize(code);
	classify_tokens(token);

	bool ast_pass=false;
	const Token *out=ast_token_cmp(
		token, (Combo[]) {
			{ .tok = TOKEN_BRACKET_OPEN },
			{ .tok = TOKEN_ANY_NON_BRACKET_TOKEN },
			{ .tok = TOKEN_BRACKET_CLOSE },
			{0}
		},
		&ast_pass
	);

	const bool pass=(
		ast_pass &&
		token->next->next == out
	);

	free_tokens(token);
	return pass;
})

static Combo optional_combo[] = {
	{ .tok = TOKEN_BRACKET_OPEN },
	{ .tok = TOKEN_IDENTIFIER, .rule = RULE_OPTIONAL },
	{ .tok = TOKEN_BRACKET_CLOSE },
	{0}
};

TEST(ast_token_cmp_optional_combo, {
	Token *token_with=tokenize(U"[ x ]");
	classify_tokens(token_with);

	Token *token_without=tokenize(U"[ ]");
	classify_tokens(token_without);

	bool ast_pass_with=false;
	const Token *out_with=ast_token_cmp(token_with, optional_combo, &ast_pass_with);

	bool ast_pass_without=false;
	const Token *out_without=ast_token_cmp(token_without, optional_combo, &ast_pass_without);

	const bool pass=(
		ast_pass_with &&
		token_with->next->next == out_with &&
		ast_pass_without &&
		token_without->next == out_without
	);

	free_tokens(token_with);
	free_tokens(token_without);
	return pass;
})

TEST(push_ast_node, {
	const char32_t *code=U"x: int = 0";
	Token *token=tokenize(code);
	Token *last=token;

	classify_tokens(token);

	AstNode *node=make_ast_node();
	AstNode *tmp=node;

	bool ast_pass=false;
	token=ast_token_cmp(token, (Combo[]) {
		{ .tok = TOKEN_NEW_IDENTIFIER },
		{ .tok = TOKEN_TYPE },
		{ .tok = TOKEN_OPER_EQUAL },
		{ .tok = TOKEN_INT_CONST },
		{0}
	}, &ast_pass);

	push_ast_node(token, &last, AST_NODE_VAR_DEF, &node);

	const bool pass=(
		ast_pass &&
		!tmp->last &&
		tmp->next==node &&
		tmp->next->last==tmp
	);

	free_tokens(token);
	return pass;
})

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

TEST(make_ast_tree_with_nothing, {
	const char32_t *error = NULL;
	AstNode *node=make_ast_tree(U"", &error);

	const bool pass=(
		error &&
		c32scmp(ERR_UNEXPECTED_TOKEN_(U""), error) &&
		node==NULL
	);

	free(node);
	return pass;
})

TEST(make_ast_tree_var_assign, {
	TEST_AST_TREE(U"x =", AST_NODE_VAR_ASSIGN, 0, 3);
})

TEST(make_ast_tree_import, {
	TEST_AST_TREE(U"import x from \"y\"", AST_NODE_IMPORT, 0, 16);
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

TEST(make_ast_tree_if, {
	const char32_t *code=U"if true [ return 1 ]";
	const char32_t *error = NULL;
	AstNode *node=make_ast_tree(code, &error);

	const bool pass=(
		!error &&
		node->node_type==AST_NODE_IF &&
		node->token->begin==code &&
		node->token_end->end==(code + 7) &&
		!node->last &&
		node->child &&
		node->child->token->begin==(code + 10) &&
		node->child->token_end->end==(code + 18) &&
		!node->next
	);
	free(node);
	return pass;
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
	TEST_AST_TREE(U"'x'", AST_NODE_RUNE_CONST, 1, 2);
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

TEST(make_ast_tree_bad_recursive_combo_fails, {
	TEST_AST_TREE(U"[", AST_NODE_UNKNOWN, 0, 0);
})

TEST(free_ast_tree, {
	const char32_t *error = NULL;
	AstNode *node=make_ast_tree(U"hello world", &error);

	free_ast_tree(node);
	return true;
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
	test_make_ast_tree_var_add,
	test_make_ast_tree_var_sub,
	test_make_ast_tree_var_mult,
	test_make_ast_tree_var_div,
	test_make_ast_tree_return,
	test_make_ast_tree_return_var,
	test_make_ast_tree_no_param_func,
	test_make_ast_tree_one_param_func,
	test_make_ast_tree_if,
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
