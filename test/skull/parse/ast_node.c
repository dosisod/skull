#include <stdbool.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/parse/ast_node.c"
#include "skull/parse/classify.h"

#include "test/skull/parse/ast_node.h"
#include "test/testing.h"

static bool ast_tree_expr_fixture(const char32_t *, ExprType, unsigned, unsigned);
static bool ast_tree_fixture(const char32_t *, NodeType, unsigned, unsigned);

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

	ParserCtx ctx = {
		.node = node,
		.token = token
	};

	push_ast_node(&ctx, last, AST_NODE_VAR_DEF);

	ASSERT_FALSEY(tmp->last);
	ASSERT_EQUAL(tmp->next, ctx.node);
	ASSERT_EQUAL(tmp->next->last, tmp);

	free_ast_tree(tmp);

	PASS
}

bool test_parse_ast_tree_variable_def() {
	return ast_tree_fixture(U"x: Int = 0", AST_NODE_VAR_DEF, 0, 8);
}

bool test_parse_ast_tree_mutable_variable_def() {
	return ast_tree_fixture(U"mut x: Int = 0", AST_NODE_VAR_DEF, 0, 12);
}

bool test_parse_ast_tree_auto_variable_def() {
	return ast_tree_fixture(U"x := 0", AST_NODE_VAR_DEF, 0, 4);
}

bool test_parse_ast_tree_auto_mutable_variable_def() {
	return ast_tree_fixture(U"mut x := 0", AST_NODE_VAR_DEF, 0, 8);
}

bool test_parse_ast_tree_bool_expr() {
	return ast_tree_expr_fixture(U"true", EXPR_CONST, 0, 4);
}

bool test_parse_ast_tree_float_expr() {
	return ast_tree_expr_fixture(U"3.14", EXPR_CONST, 0, 4);
}

bool test_parse_ast_tree_str_expr() {
	return ast_tree_expr_fixture(U"\"asdf\"", EXPR_CONST, 0, 6);
}

bool test_parse_ast_tree_rune_expr() {
	return ast_tree_expr_fixture(U"'x'", EXPR_CONST, 0, 3);
}

bool test_parse_ast_tree_and_expr() {
	return ast_tree_expr_fixture(U"true and false", EXPR_AND, 0, 14);
}

bool test_parse_ast_tree_greater_then_or_equal_expr() {
	return ast_tree_expr_fixture(U"1 >= 0", EXPR_GTR_THAN_EQ, 0, 5);
}

bool test_parse_ast_tree_greater_then_expr() {
	return ast_tree_expr_fixture(U"1 > 0", EXPR_GTR_THAN, 0, 5);
}

bool test_parse_ast_tree_isnt_expr() {
	return ast_tree_expr_fixture(U"1 isnt 2", EXPR_ISNT, 0, 8);
}

bool test_parse_ast_tree_is_expr() {
	return ast_tree_expr_fixture(U"1 is 2", EXPR_IS, 0, 6);
}

bool test_parse_ast_tree_less_then_or_equal_expr() {
	return ast_tree_expr_fixture(U"1 <= 0", EXPR_LESS_THAN_EQ, 0, 5);
}

bool test_parse_ast_tree_less_then_expr() {
	return ast_tree_expr_fixture(U"1 < 0", EXPR_LESS_THAN, 0, 5);
}


bool test_parse_ast_tree_not_expr() {
	return ast_tree_expr_fixture(U"not false", EXPR_NOT, 0, 9);
}

bool test_parse_ast_tree_or_expr() {
	return ast_tree_expr_fixture(U"true or false", EXPR_OR, 0, 13);
}

bool test_parse_ast_tree_xor_expr() {
	return ast_tree_expr_fixture(U"true xor false", EXPR_XOR, 0, 14);
}

bool test_parse_ast_tree_export_var() {
	return ast_tree_fixture(U"export x := 1", AST_NODE_VAR_DEF, 0, 11);
}

bool test_parse_ast_tree_str_with_comment() {
	return ast_tree_expr_fixture(U"\"$ \"", EXPR_CONST, 0, 4);
}

bool test_parse_ast_tree_block_comment() {
	return ast_tree_fixture(U"#{ #}", AST_NODE_COMMENT, 0, 5);
}

bool test_parse_ast_tree_lshift_expr() {
	return ast_tree_expr_fixture(U"1 << 2", EXPR_LSHIFT, 0, 6);
}

bool test_parse_ast_tree_rshift_expr() {
	return ast_tree_expr_fixture(U"1 >> 2", EXPR_RSHIFT, 0, 6);
}

bool test_parse_ast_tree_mod_expr() {
	return ast_tree_expr_fixture(U"1 mod 2", EXPR_MOD, 0, 7);
}

bool test_parse_ast_tree_mult_expr() {
	return ast_tree_expr_fixture(U"1 * 2", EXPR_MULT, 0, 5);
}

bool test_parse_ast_tree_div_expr() {
	return ast_tree_expr_fixture(U"1 / 2", EXPR_DIV, 0, 5);
}

bool test_parse_ast_tree_pow_expr() {
	return ast_tree_expr_fixture(U"1 ^ 2", EXPR_POW, 0, 5);
}

bool test_parse_ast_tree_negative_unary_expr() {
	return ast_tree_expr_fixture(U"- 1", EXPR_UNARY_NEG, 0, 3);
}

bool test_parse_ast_tree_paren_expr() {
	return ast_tree_expr_fixture(U"(1)", EXPR_CONST, 0, 3);
}

bool test_parse_ast_tree_sub_expr() {
	return ast_tree_expr_fixture(U"1 - 2", EXPR_SUB, 0, 5);
}

bool test_parse_ast_tree_var_assign() {
	return ast_tree_fixture(U"x = 0", AST_NODE_VAR_ASSIGN, 0, 3);
}

bool test_parse_ast_tree_external() {
	return ast_tree_fixture(U"external x()\n", AST_NODE_FUNCTION_PROTO, 0, 12);
}

bool test_parse_ast_tree_function() {
	return ast_tree_fixture(U"x()", AST_NODE_EXPR, 0, 3);
}

bool test_parse_ast_tree_function_newline() {
	// TODO(dosisod): fix newline being added
	return ast_tree_fixture(U"x()\n", AST_NODE_EXPR, 0, 4);
}

bool test_parse_ast_tree_return() {
	return ast_tree_fixture(U"return 0", AST_NODE_RETURN, 0, 6);
}
bool test_parse_ast_tree_if() {
	const char32_t *const code = U"if true { noop }";
	Token *token = tokenize(code);
	if (!token) return true;

	classify_tokens(token);

	AstNode *node = parse_ast_tree(token);

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
	Token *token = tokenize(code);
	if (!token) return true;

	classify_tokens(token);

	AstNode *node = parse_ast_tree(token);

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
	return ast_tree_fixture(U"# this is a comment", AST_NODE_COMMENT, 0, 19);
}

bool test_parse_ast_tree_noop() {
	return ast_tree_fixture(U"noop", AST_NODE_NOOP, 0, 4);
}

bool test_free_ast_tree() {
	free_ast_tree(parse_ast_tree(tokenize(U"noop")));

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

static bool ast_tree_fixture(const char32_t *code, NodeType node_type, unsigned begin_offset, unsigned end_offset) {
	Token *token = tokenize(code);
	if (!token) return true;

	classify_tokens(token);

	AstNode *node = parse_ast_tree(token);
	if (!node->type && !node_type) {
		return true;
	}

	ASSERT_EQUAL(node->type, node_type);
	ASSERT_EQUAL(node->token->begin, code + begin_offset);

	if (node->token_end) {
		ASSERT_EQUAL(node->token_end->end, code + end_offset);
	}

	ASSERT_FALSEY(node->last);
	free_ast_tree(node);

	PASS
}

static bool ast_tree_expr_fixture(const char32_t *code, ExprType expr_type, unsigned begin_offset, unsigned end_offset) {
	Token *token = tokenize(code);
	if (!token) return true;

	classify_tokens(token);

	AstNode *node = parse_ast_tree(token);
	ASSERT_EQUAL(node->type, AST_NODE_EXPR);
	ASSERT_TRUTHY(node->expr);
	ASSERT_EQUAL(node->expr->oper, expr_type);
	ASSERT_EQUAL(node->token->begin, code + begin_offset);

	if (node->token_end) {
		ASSERT_EQUAL(node->token_end->end, code + end_offset);
	}

	ASSERT_FALSEY(node->last);
	free_ast_tree(node);

	PASS
}
