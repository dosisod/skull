#include <stdbool.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/parse/ast_node.c"
#include "skull/parse/classify.h"

#include "test/skull/parse/ast_node.h"
#include "test/testing.h"

static bool ast_tree_expr_fixture(const char32_t *, ExprType, unsigned, unsigned);
static bool ast_tree_fixture(const char32_t *, NodeType, unsigned, unsigned);

static bool test_make_ast_node(void) {
	AstNode *node = make_ast_node();

	ASSERT_EQUAL(node->type, AST_NODE_UNKNOWN);
	ASSERT_FALSEY(node->token);
	ASSERT_FALSEY(node->token_end);
	ASSERT_FALSEY(node->last);
	ASSERT_FALSEY(node->next);

	free_ast_tree(node);

	PASS
}

static bool test_push_ast_node(void) {
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
	free_tokens(token);

	PASS
}

static bool test_parse_ast_tree_variable_def(void) {
	return ast_tree_fixture(U"x: Int = 0", AST_NODE_VAR_DEF, 0, 8);
}

static bool test_parse_ast_tree_mutable_variable_def(void) {
	return ast_tree_fixture(U"mut x: Int = 0", AST_NODE_VAR_DEF, 0, 12);
}

static bool test_parse_ast_tree_auto_variable_def(void) {
	return ast_tree_fixture(U"x := 0", AST_NODE_VAR_DEF, 0, 4);
}

static bool test_parse_ast_tree_auto_mutable_variable_def(void) {
	return ast_tree_fixture(U"mut x := 0", AST_NODE_VAR_DEF, 0, 8);
}

static bool test_parse_ast_tree_bool_expr(void) {
	return ast_tree_expr_fixture(U"true", EXPR_CONST, 0, 4);
}

static bool test_parse_ast_tree_float_expr(void) {
	return ast_tree_expr_fixture(U"3.14", EXPR_CONST, 0, 4);
}

static bool test_parse_ast_tree_str_expr(void) {
	return ast_tree_expr_fixture(U"\"asdf\"", EXPR_CONST, 0, 6);
}

static bool test_parse_ast_tree_rune_expr(void) {
	return ast_tree_expr_fixture(U"'x'", EXPR_CONST, 0, 3);
}

static bool test_parse_ast_tree_and_expr(void) {
	return ast_tree_expr_fixture(U"true and false", EXPR_AND, 0, 14);
}

static bool test_parse_ast_tree_greater_then_or_equal_expr(void) {
	return ast_tree_expr_fixture(U"1 >= 0", EXPR_GTR_THAN_EQ, 0, 5);
}

static bool test_parse_ast_tree_greater_then_expr(void) {
	return ast_tree_expr_fixture(U"1 > 0", EXPR_GTR_THAN, 0, 5);
}

static bool test_parse_ast_tree_isnt_expr(void) {
	return ast_tree_expr_fixture(U"1 isnt 2", EXPR_ISNT, 0, 8);
}

static bool test_parse_ast_tree_is_expr(void) {
	return ast_tree_expr_fixture(U"1 is 2", EXPR_IS, 0, 6);
}

static bool test_parse_ast_tree_less_then_or_equal_expr(void) {
	return ast_tree_expr_fixture(U"1 <= 0", EXPR_LESS_THAN_EQ, 0, 5);
}

static bool test_parse_ast_tree_less_then_expr(void) {
	return ast_tree_expr_fixture(U"1 < 0", EXPR_LESS_THAN, 0, 5);
}

static bool test_parse_ast_tree_not_expr(void) {
	return ast_tree_expr_fixture(U"not false", EXPR_NOT, 0, 9);
}

static bool test_parse_ast_tree_or_expr(void) {
	return ast_tree_expr_fixture(U"true or false", EXPR_OR, 0, 13);
}

static bool test_parse_ast_tree_xor_expr(void) {
	return ast_tree_expr_fixture(U"true xor false", EXPR_XOR, 0, 14);
}

static bool test_parse_ast_tree_export_var(void) {
	return ast_tree_fixture(U"export x := 1", AST_NODE_VAR_DEF, 0, 11);
}

static bool test_parse_ast_tree_str_with_comment(void) {
	return ast_tree_expr_fixture(U"\"$ \"", EXPR_CONST, 0, 4);
}

static bool test_parse_ast_tree_block_comment(void) {
	return ast_tree_fixture(U"#{ #}", AST_NODE_COMMENT, 0, 5);
}

static bool test_parse_ast_tree_lshift_expr(void) {
	return ast_tree_expr_fixture(U"1 << 2", EXPR_LSHIFT, 0, 6);
}

static bool test_parse_ast_tree_rshift_expr(void) {
	return ast_tree_expr_fixture(U"1 >> 2", EXPR_RSHIFT, 0, 6);
}

static bool test_parse_ast_tree_mod_expr(void) {
	return ast_tree_expr_fixture(U"1 mod 2", EXPR_MOD, 0, 7);
}

static bool test_parse_ast_tree_mult_expr(void) {
	return ast_tree_expr_fixture(U"1 * 2", EXPR_MULT, 0, 5);
}

static bool test_parse_ast_tree_div_expr(void) {
	return ast_tree_expr_fixture(U"1 / 2", EXPR_DIV, 0, 5);
}

static bool test_parse_ast_tree_pow_expr(void) {
	return ast_tree_expr_fixture(U"1 ^ 2", EXPR_POW, 0, 5);
}

static bool test_parse_ast_tree_negative_unary_expr(void) {
	return ast_tree_expr_fixture(U"- 1", EXPR_UNARY_NEG, 0, 3);
}

static bool test_parse_ast_tree_paren_expr(void) {
	return ast_tree_expr_fixture(U"(1)", EXPR_CONST, 0, 3);
}

static bool test_parse_ast_tree_sub_expr(void) {
	return ast_tree_expr_fixture(U"1 - 2", EXPR_SUB, 0, 5);
}

static bool test_parse_ast_tree_var_assign(void) {
	return ast_tree_fixture(U"x = 0", AST_NODE_VAR_ASSIGN, 0, 3);
}

static bool test_parse_ast_tree_external(void) {
	return ast_tree_fixture(U"external x()\n", AST_NODE_FUNCTION_PROTO, 0, 12);
}

static bool test_parse_ast_tree_function(void) {
	return ast_tree_fixture(U"x()", AST_NODE_EXPR, 0, 3);
}

static bool test_parse_ast_tree_function_newline(void) {
	// TODO(dosisod): fix newline being added
	return ast_tree_fixture(U"x()\n", AST_NODE_EXPR, 0, 4);
}

static bool test_parse_ast_tree_return(void) {
	return ast_tree_fixture(U"return 0", AST_NODE_RETURN, 0, 6);
}

#define AST_NODE_FIXTURE(_code) \
	const char32_t *code = (_code); \
	Token *token = tokenize(code); \
	if (!token) return true; \
	classify_tokens(token); \
	AstNode *node = parse_ast_tree(token)

static bool test_parse_ast_tree_if(void) {
	AST_NODE_FIXTURE(U"if true { noop }");

	ASSERT_EQUAL(node->type, AST_NODE_IF);
	ASSERT_EQUAL(node->token->begin, code);
	ASSERT_EQUAL(node->token_end->len, 2);
	ASSERT_FALSEY(node->last);
	ASSERT_FALSEY(node->next);
	ASSERT_TRUTHY(node->expr);
	ASSERT_TRUTHY(node->child);

	free_ast_tree(node);
	free_tokens(token);

	PASS
}

static bool test_parse_ast_tree_if_with_var(void) {
	AST_NODE_FIXTURE(U"if x { noop }");

	ASSERT_EQUAL(node->type, AST_NODE_IF);
	ASSERT_EQUAL(node->token->begin, code);
	ASSERT_EQUAL(node->token_end->len, 2);
	ASSERT_FALSEY(node->last);
	ASSERT_FALSEY(node->next);
	ASSERT_TRUTHY(node->expr);
	ASSERT_TRUTHY(node->child);

	free_ast_tree(node);
	free_tokens(token);

	PASS
}

static bool test_parse_ast_tree_namespace(void) {
	AST_NODE_FIXTURE(U"namespace x { noop }");

	ASSERT_EQUAL(node->type, AST_NODE_NAMESPACE);
	ASSERT_EQUAL(node->token->begin, code);
	ASSERT_EQUAL(node->token_end->len, 1);
	ASSERT_FALSEY(node->last);
	ASSERT_FALSEY(node->next);
	ASSERT_TRUTHY(node->child);
	ASSERT_EQUAL(node->child->type, AST_NODE_NOOP);

	free_ast_tree(node);
	free_tokens(token);

	PASS
}

static bool test_parse_ast_tree_comment(void) {
	return ast_tree_fixture(U"# this is a comment", AST_NODE_COMMENT, 0, 19);
}

static bool test_parse_ast_tree_noop(void) {
	return ast_tree_fixture(U"noop", AST_NODE_NOOP, 0, 4);
}

static bool test_parse_ast_tree_break(void) {
	return ast_tree_fixture(U"break", AST_NODE_BREAK, 0, 5);
}

static bool test_parse_ast_tree_continue(void) {
	return ast_tree_fixture(U"continue", AST_NODE_CONTINUE, 0, 8);
}

static bool test_free_ast_tree(void) {
	Token *token = tokenize(U"noop");

	free_ast_tree(parse_ast_tree(token));

	free_tokens(token);

	PASS
}

static bool test_ooo_unary_association(void) {
	AST_NODE_FIXTURE(U"- 1 + 2");

	ASSERT_EQUAL(node->type, AST_NODE_EXPR);
	ASSERT_EQUAL(node->expr->oper, EXPR_ADD);
	ASSERT_EQUAL(node->expr->lhs.expr->oper, EXPR_UNARY_NEG);
	ASSERT_EQUAL(node->expr->rhs->oper, EXPR_CONST);

	free_ast_tree(node);
	free_tokens(token);

	PASS
}

static bool test_ooo_paren_expr(void) {
	AST_NODE_FIXTURE(U"(1 + 2)");

	ASSERT_EQUAL(node->type, AST_NODE_EXPR);
	ASSERT_EQUAL(node->expr->oper, EXPR_ADD);
	ASSERT_EQUAL(node->expr->lhs.expr->oper, EXPR_CONST);
	ASSERT_EQUAL(node->expr->rhs->oper, EXPR_CONST);

	free_ast_tree(node);
	free_tokens(token);

	PASS
}

static bool test_ooo_group_higher_precedence_1(void) {
	AST_NODE_FIXTURE(U"1 * 2 + 3");

	ASSERT_EQUAL(node->type, AST_NODE_EXPR);
	ASSERT_EQUAL(node->expr->oper, EXPR_ADD);
	ASSERT_EQUAL(node->expr->lhs.expr->oper, EXPR_MULT);
	ASSERT_EQUAL(node->expr->lhs.expr->lhs.expr->oper, EXPR_CONST);
	ASSERT_EQUAL(node->expr->lhs.expr->rhs->oper, EXPR_CONST);
	ASSERT_EQUAL(node->expr->rhs->oper, EXPR_CONST);

	free_ast_tree(node);
	free_tokens(token);

	PASS
}

static bool test_ooo_group_higher_precedence_2(void) {
	AST_NODE_FIXTURE(U"1 + 2 * 3");

	ASSERT_EQUAL(node->type, AST_NODE_EXPR);
	ASSERT_EQUAL(node->expr->oper, EXPR_ADD);
	ASSERT_EQUAL(node->expr->lhs.expr->oper, EXPR_CONST);
	ASSERT_EQUAL(node->expr->rhs->oper, EXPR_MULT);
	ASSERT_EQUAL(node->expr->rhs->lhs.expr->oper, EXPR_CONST);
	ASSERT_EQUAL(node->expr->rhs->rhs->oper, EXPR_CONST);

	free_ast_tree(node);
	free_tokens(token);

	PASS
}

static bool test_ooo_group_higher_precedence_3(void) {
	AST_NODE_FIXTURE(U"1 + 2 + 3 * 4");

	ASSERT_EQUAL(node->type, AST_NODE_EXPR);
	ASSERT_EQUAL(node->expr->oper, EXPR_ADD);
	ASSERT_EQUAL(node->expr->lhs.expr->oper, EXPR_ADD);
	ASSERT_EQUAL(node->expr->lhs.expr->lhs.expr->oper, EXPR_CONST);
	ASSERT_EQUAL(node->expr->lhs.expr->rhs->oper, EXPR_CONST);
	ASSERT_EQUAL(node->expr->rhs->oper, EXPR_MULT);
	ASSERT_EQUAL(node->expr->rhs->lhs.expr->oper, EXPR_CONST);
	ASSERT_EQUAL(node->expr->rhs->rhs->oper, EXPR_CONST);

	free_ast_tree(node);
	free_tokens(token);

	PASS
}

static bool test_ooo_group_higher_precedence_4(void) {
	AST_NODE_FIXTURE(U"1 + 2 * 3 + 4");

	ASSERT_EQUAL(node->type, AST_NODE_EXPR);
	ASSERT_EQUAL(node->expr->oper, EXPR_ADD);
	ASSERT_EQUAL(node->expr->rhs->oper, EXPR_CONST);
	ASSERT_EQUAL(node->expr->lhs.expr->oper, EXPR_ADD);
	ASSERT_EQUAL(node->expr->lhs.expr->lhs.expr->oper, EXPR_CONST);
	ASSERT_EQUAL(node->expr->lhs.expr->rhs->oper, EXPR_MULT);
	ASSERT_EQUAL(node->expr->lhs.expr->rhs->lhs.expr->oper, EXPR_CONST);
	ASSERT_EQUAL(node->expr->lhs.expr->rhs->rhs->oper, EXPR_CONST);

	free_ast_tree(node);
	free_tokens(token);

	PASS
}

static bool test_ooo_group_same_precedence(void) {
	AST_NODE_FIXTURE(U"1 * 2 / 3 + 4");

	ASSERT_EQUAL(node->type, AST_NODE_EXPR);
	ASSERT_EQUAL(node->expr->oper, EXPR_ADD);
	ASSERT_EQUAL(node->expr->rhs->oper, EXPR_CONST);
	ASSERT_EQUAL(node->expr->lhs.expr->oper, EXPR_DIV);
	ASSERT_EQUAL(node->expr->lhs.expr->rhs->oper, EXPR_CONST);
	ASSERT_EQUAL(node->expr->lhs.expr->lhs.expr->oper, EXPR_MULT);
	ASSERT_EQUAL(node->expr->lhs.expr->lhs.expr->lhs.expr->oper, EXPR_CONST);
	ASSERT_EQUAL(node->expr->lhs.expr->lhs.expr->rhs->oper, EXPR_CONST);

	free_ast_tree(node);
	free_tokens(token);

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
		test_parse_ast_tree_function_newline,
		test_parse_ast_tree_return,
		test_parse_ast_tree_if,
		test_parse_ast_tree_if_with_var,
		test_parse_ast_tree_namespace,
		test_parse_ast_tree_comment,
		test_parse_ast_tree_noop,
		test_parse_ast_tree_break,
		test_parse_ast_tree_continue,
		test_free_ast_tree,
		test_ooo_unary_association,
		test_ooo_paren_expr,
		test_ooo_group_higher_precedence_1,
		test_ooo_group_higher_precedence_2,
		test_ooo_group_higher_precedence_3,
		test_ooo_group_higher_precedence_4,
		test_ooo_group_same_precedence
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
		ASSERT_EQUAL(
			(
				node->token_end->begin -
				node->token->begin +
				node->token_end->len
			),
			end_offset
		);
	}

	ASSERT_FALSEY(node->last);
	free_ast_tree(node);
	free_tokens(token);

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
		ASSERT_EQUAL(node->token_end->len, end_offset);
	}

	ASSERT_FALSEY(node->last);
	free_ast_tree(node);
	free_tokens(token);

	PASS
}
