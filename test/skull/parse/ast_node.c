#include <stdbool.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/parse/ast_node.c"
#include "skull/parse/classify.h"

#include "test/skull/parse/ast_node.h"
#include "test/testing.h"

bool test_make_ast_node() {
	AstNode *node = make_ast_node();

	ASSERT_EQUAL(node->type, AST_NODE_UNKNOWN);
	ASSERT_FALSEY(node->token);
	ASSERT_FALSEY(node->token_end);
	ASSERT_FALSEY(node->last);
	ASSERT_FALSEY(node->next);

	free_ast_tree(node);

	PASS
}

bool test_push_ast_node() {
	Token *token = tokenize(U"x: Int = 0");
	Token *last = token;

	classify_tokens(token);

	AstNode *node = make_ast_node();
	AstNode *tmp = node;

	push_ast_node(token, last, AST_NODE_VAR_DEF, &node);

	ASSERT_FALSEY(tmp->last);
	ASSERT_EQUAL(tmp->next, node);
	ASSERT_EQUAL(tmp->next->last, tmp);

	free_ast_tree(tmp);

	PASS
}

bool test_parse_ast_tree_variable_def() {
	TEST_AST_TREE(U"x: Int = 0", AST_NODE_VAR_DEF, 0, 8);

	PASS
}

bool test_parse_ast_tree_mutable_variable_def() {
	TEST_AST_TREE(U"mut x: Int = 0", AST_NODE_VAR_DEF, 0, 12);

	PASS
}

bool test_parse_ast_tree_auto_variable_def() {
	TEST_AST_TREE(U"x := 0", AST_NODE_VAR_DEF, 0, 4);

	PASS
}

bool test_parse_ast_tree_auto_mutable_variable_def() {
	TEST_AST_TREE(U"mut x := 0", AST_NODE_VAR_DEF, 0, 8);

	PASS
}

bool test_parse_ast_tree_bool_expr() {
	TEST_AST_TREE_EXPR(U"true", EXPR_CONST, 0, 4);

	PASS
}

bool test_parse_ast_tree_float_expr() {
	TEST_AST_TREE_EXPR(U"3.14", EXPR_CONST, 0, 4);

	PASS
}

bool test_parse_ast_tree_str_expr() {
	TEST_AST_TREE_EXPR(U"\"asdf\"", EXPR_CONST, 0, 6);

	PASS
}

bool test_parse_ast_tree_rune_expr() {
	TEST_AST_TREE_EXPR(U"'x'", EXPR_CONST, 0, 3);

	PASS
}

bool test_parse_ast_tree_and_expr() {
	TEST_AST_TREE_EXPR(U"true and false", EXPR_AND, 0, 14);

	PASS
}

bool test_parse_ast_tree_greater_then_or_equal_expr() {
	TEST_AST_TREE_EXPR(U"1 >= 0", EXPR_GTR_THAN_EQ, 0, 5);

	PASS
}

bool test_parse_ast_tree_greater_then_expr() {
	TEST_AST_TREE_EXPR(U"1 > 0", EXPR_GTR_THAN, 0, 5);

	PASS
}

bool test_parse_ast_tree_isnt_expr() {
	TEST_AST_TREE_EXPR(U"1 isnt 2", EXPR_ISNT, 0, 8);

	PASS
}

bool test_parse_ast_tree_is_expr() {
	TEST_AST_TREE_EXPR(U"1 is 2", EXPR_IS, 0, 6);

	PASS
}

bool test_parse_ast_tree_less_then_or_equal_expr() {
	TEST_AST_TREE_EXPR(U"1 <= 0", EXPR_LESS_THAN_EQ, 0, 5);

	PASS
}

bool test_parse_ast_tree_less_then_expr() {
	TEST_AST_TREE_EXPR(U"1 < 0", EXPR_LESS_THAN, 0, 5);

	PASS
}


bool test_parse_ast_tree_not_expr() {
	TEST_AST_TREE_EXPR(U"not false", EXPR_NOT, 0, 9);

	PASS
}

bool test_parse_ast_tree_or_expr() {
	TEST_AST_TREE_EXPR(U"true or false", EXPR_OR, 0, 13);

	PASS
}

bool test_parse_ast_tree_xor_expr() {
	TEST_AST_TREE_EXPR(U"true xor false", EXPR_XOR, 0, 14);

	PASS
}

bool test_parse_ast_tree_export_var() {
	TEST_AST_TREE(U"export x := 1", AST_NODE_VAR_DEF, 0, 11);

	PASS
}

bool test_parse_ast_tree_str_with_comment() {
	TEST_AST_TREE_EXPR(U"\"$ \"", EXPR_CONST, 0, 4);

	PASS
}

bool test_parse_ast_tree_block_comment() {
	TEST_AST_TREE(U"#{ #}", AST_NODE_COMMENT, 0, 5);

	PASS
}

bool test_parse_ast_tree_lshift_expr() {
	TEST_AST_TREE_EXPR(U"1 << 2", EXPR_LSHIFT, 0, 6);

	PASS
}

bool test_parse_ast_tree_rshift_expr() {
	TEST_AST_TREE_EXPR(U"1 >> 2", EXPR_RSHIFT, 0, 6);

	PASS
}

bool test_parse_ast_tree_mod_expr() {
	TEST_AST_TREE_EXPR(U"1 mod 2", EXPR_MOD, 0, 7);

	PASS
}

bool test_parse_ast_tree_mult_expr() {
	TEST_AST_TREE_EXPR(U"1 * 2", EXPR_MULT, 0, 5);

	PASS
}

bool test_parse_ast_tree_div_expr() {
	TEST_AST_TREE_EXPR(U"1 / 2", EXPR_DIV, 0, 5);

	PASS
}

bool test_parse_ast_tree_pow_expr() {
	TEST_AST_TREE_EXPR(U"1 ^ 2", EXPR_POW, 0, 5);

	PASS
}

bool test_parse_ast_tree_negative_unary_expr() {
	TEST_AST_TREE_EXPR(U"- 1", EXPR_UNARY_NEG, 0, 3);

	PASS
}

bool test_parse_ast_tree_paren_expr() {
	TEST_AST_TREE_EXPR(U"(1)", EXPR_CONST, 0, 3);

	PASS
}

bool test_parse_ast_tree_sub_expr() {
	TEST_AST_TREE_EXPR(U"1 - 2", EXPR_SUB, 0, 5);

	PASS
}

bool test_parse_ast_tree_var_assign() {
	TEST_AST_TREE(U"x = 0", AST_NODE_VAR_ASSIGN, 0, 3);

	PASS
}

bool test_parse_ast_tree_external() {
	TEST_AST_TREE(U"external x()\n", AST_NODE_FUNCTION_PROTO, 0, 12);

	PASS
}

bool test_parse_ast_tree_function() {
	{
		TEST_AST_TREE(U"x()", AST_NODE_EXPR, 0, 3);
	}
	{
		// TODO(dosisod): fix newline being added
		TEST_AST_TREE(U"x()\n", AST_NODE_EXPR, 0, 4);
	}

	PASS
}

bool test_parse_ast_tree_return() {
	TEST_AST_TREE(U"return 0", AST_NODE_RETURN, 0, 6);

	PASS
}
bool test_parse_ast_tree_if() {
	const char32_t *const code = U"if true { noop }";
	AstNode *node = parse_ast_tree(code);

	ASSERT_EQUAL(node->type, AST_NODE_IF);
	ASSERT_EQUAL(node->token->begin, code);
	ASSERT_EQUAL(node->token_end->end, code + 2);
	ASSERT_FALSEY(node->last);
	ASSERT_FALSEY(node->next);
	ASSERT_TRUTHY(node->expr_node);
	ASSERT_TRUTHY(node->child);

	free_ast_tree(node);

	PASS
}

bool test_parse_ast_tree_if_with_var() {
	const char32_t *const code = U"if x { noop }";
	AstNode *node = parse_ast_tree(code);

	ASSERT_EQUAL(node->type, AST_NODE_IF);
	ASSERT_EQUAL(node->token->begin, code);
	ASSERT_EQUAL(node->token_end->end, code + 2);
	ASSERT_FALSEY(node->last);
	ASSERT_FALSEY(node->next);
	ASSERT_TRUTHY(node->expr_node);
	ASSERT_TRUTHY(node->child);

	free_ast_tree(node);

	PASS
}

bool test_parse_ast_tree_comment() {
	TEST_AST_TREE(U"# this is a comment", AST_NODE_COMMENT, 0, 19);

	PASS
}

bool test_parse_ast_tree_noop() {
	TEST_AST_TREE(U"noop", AST_NODE_NOOP, 0, 4);

	PASS
}

bool test_free_ast_tree() {
	free_ast_tree(parse_ast_tree(U"# whatever"));

	PASS
}

void ast_node_test_self(bool *pass) {
	RUN_ALL(
		test_make_ast_node,
		test_push_ast_node,
		test_parse_ast_tree_variable_def,
		test_parse_ast_tree_mutable_variable_def,
		test_parse_ast_tree_auto_variable_def,
		test_parse_ast_tree_auto_mutable_variable_def,
		test_parse_ast_tree_bool_expr,
		test_parse_ast_tree_float_expr,
		test_parse_ast_tree_str_expr,
		test_parse_ast_tree_rune_expr,
		test_parse_ast_tree_and_expr,
		test_parse_ast_tree_greater_then_or_equal_expr,
		test_parse_ast_tree_greater_then_expr,
		test_parse_ast_tree_isnt_expr,
		test_parse_ast_tree_is_expr,
		test_parse_ast_tree_less_then_or_equal_expr,
		test_parse_ast_tree_less_then_expr,
		test_parse_ast_tree_not_expr,
		test_parse_ast_tree_or_expr,
		test_parse_ast_tree_xor_expr,
		test_parse_ast_tree_export_var,
		test_parse_ast_tree_str_with_comment,
		test_parse_ast_tree_block_comment,
		test_parse_ast_tree_lshift_expr,
		test_parse_ast_tree_rshift_expr,
		test_parse_ast_tree_mod_expr,
		test_parse_ast_tree_mult_expr,
		test_parse_ast_tree_div_expr,
		test_parse_ast_tree_pow_expr,
		test_parse_ast_tree_negative_unary_expr,
		test_parse_ast_tree_paren_expr,
		test_parse_ast_tree_sub_expr,
		test_parse_ast_tree_var_assign,
		test_parse_ast_tree_external,
		test_parse_ast_tree_function,
		test_parse_ast_tree_return,
		test_parse_ast_tree_if,
		test_parse_ast_tree_if_with_var,
		test_parse_ast_tree_comment,
		test_parse_ast_tree_noop,
		test_free_ast_tree
	)
}
