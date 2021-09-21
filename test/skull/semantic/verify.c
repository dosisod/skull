#include <stdbool.h>
#include <stdlib.h>

#include "skull/common/errors.h"
#include "skull/parse/ast_node.h"
#include "skull/parse/classify.h"
#include "skull/parse/token.h"
#include "skull/semantic/entry.h"
#include "skull/semantic/expr.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/types.h"

#include "test/testing.h"

#include "./macros.h"


static bool validate_binary_expr_fixture(AstNode *, const char *);
static Token *tokenize_fixture(const char32_t *);

bool test_validate_int_expr() {
	Token *token = tokenize_fixture(U"1");
	AstNode *node = AST_SIMPLE_EXPR(token);

	const bool pass = validate_expr(node);

	free_tokens(token);
	return pass;
}

bool test_validate_int_overflow() {
	Token *token = tokenize_fixture(U"99999999999999999999999999999999");
	AstNode *node = AST_SIMPLE_EXPR(token);

	ASSERT_FALSEY(validate_expr(node));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: overflow occurred while parsing \"99999999999999999999999999999999\"\n"
	));

	free_tokens(token);
	PASS
}

bool test_validate_int_underflow() {
	Token *token = tokenize_fixture(U"-99999999999999999999999999999999");
	AstNode *node = AST_SIMPLE_EXPR(token);

	ASSERT_FALSEY(validate_expr(node));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: overflow occurred while parsing \"-99999999999999999999999999999999\"\n"
	));

	free_tokens(token);
	PASS
}

bool test_validate_float_overflow() {
	Token *token = tokenize_fixture(U"999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0");
	AstNode *node = AST_SIMPLE_EXPR(token);

	ASSERT_FALSEY(validate_expr(node));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: overflow occurred while parsing \"999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0\"\n"
	));

	free_tokens(token);
	PASS
}

bool test_validate_missing_function_decl() {
	Token *token = tokenize_fixture(U"x()");

	AstNode *node = &(AstNode){
		.type = AST_NODE_EXPR,
		.token = token,
		.expr = AST_NO_ARG_FUNC_EXPR(token)
	};

	ASSERT_FALSEY(validate_expr(node));
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: function \"x\" missing declaration\n"
	));

	free_tokens(token);
	PASS
}

bool test_validate_trailing_expr() {
	Token *token = tokenize_fixture(U"1");
	AstNode *node = AST_SIMPLE_EXPR(token);

	ASSERT_FALSEY(validate_ast_tree(node));
	free(node->expr->value.str);

	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: expression cannot be used on its own\n"
	));

	free_tokens(token);
	PASS
}

bool test_validate_invalid_and() {
	Token *token = tokenize_fixture(U"1 and 1");
	AstNode *node = AST_SIMPLE_BINARY_EXPR(token, EXPR_AND);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 1: expected type \"Bool\", got \"Int\"\n"
	);
}

bool test_validate_invalid_or() {
	Token *token = tokenize_fixture(U"1 or 1");
	AstNode *node = AST_SIMPLE_BINARY_EXPR(token, EXPR_OR);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 1: expected type \"Bool\", got \"Int\"\n"
	);
}

bool test_validate_invalid_xor() {
	Token *token = tokenize_fixture(U"1 xor 1");
	AstNode *node = AST_SIMPLE_BINARY_EXPR(token, EXPR_XOR);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 1: expected type \"Bool\", got \"Int\"\n"
	);
}

bool test_validate_div_by_zero() {
	Token *token = tokenize_fixture(U"1 / 0");
	AstNode *node = AST_SIMPLE_BINARY_EXPR(token, EXPR_DIV);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 5: division by zero\n"
	);
}

bool test_validate_lhs_var_missing() {
	Token *token = tokenize_fixture(U"fail + 1");

	AstNode *node = &(AstNode){
		.type = AST_NODE_EXPR,
		.token = token,
		.expr = AST_BINARY_EXPR(
			AST_IDENT_EXPR(token),
			EXPR_ADD,
			AST_CONST_EXPR(token->next->next)
		)
	};

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 1: variable \"fail\" not found\n"
	);
}

bool test_validate_rhs_var_missing() {
	Token *token = tokenize_fixture(U"1 + fail");

	AstNode *node = &(AstNode){
		.type = AST_NODE_EXPR,
		.token = token,
		.expr = AST_BINARY_EXPR(
			AST_CONST_EXPR(token),
			EXPR_ADD,
			AST_IDENT_EXPR(token->next->next)
		)
	};

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 5: variable \"fail\" not found\n"
	);
}

bool test_validate_pow_type() {
	Token *token = tokenize_fixture(U"\"123\" ^ \"123\"");
	AstNode *node = AST_SIMPLE_BINARY_EXPR(token, EXPR_POW);

	const bool pass = validate_binary_expr_fixture(
		node,
		"(null): Compilation error: cannot use type \"Str\" for power operator\n"
	);

	free(node->expr->lhs.expr->value.str);
	free(node->expr->rhs->value.str);

	return pass;
}

bool test_validate_shift_no_ints() {
	Token *token = tokenize_fixture(U"1.0 << 2.0");
	AstNode *node = AST_SIMPLE_BINARY_EXPR(token, EXPR_LSHIFT);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 1: expected an integer\n"
	);
}

bool test_validate_not_oper_non_bool() {
	Token *token = tokenize_fixture(U"not 1");
	AstNode *node = AST_SIMPLE_UNARY_EXPR(token, EXPR_NOT);

	return validate_binary_expr_fixture(
		node,
		"(null): Compilation error: line 1 column 5: expected type \"Bool\", got \"Int\"\n"
	);
}

void semantic_verify_test_self(bool *pass) {
	RUN_ALL(
		test_validate_int_expr,
		test_validate_int_overflow,
		test_validate_int_underflow,
		test_validate_float_overflow,
		test_validate_missing_function_decl,
		test_validate_trailing_expr,
		test_validate_invalid_and,
		test_validate_invalid_or,
		test_validate_invalid_xor,
		test_validate_div_by_zero,
		test_validate_lhs_var_missing,
		test_validate_rhs_var_missing,
		test_validate_pow_type,
		test_validate_shift_no_ints,
		test_validate_not_oper_non_bool
	)
}

static bool validate_binary_expr_fixture(AstNode *node, const char *errors) {
	ASSERT_FALSEY(validate_expr(node));
	ASSERT_FALSEY(compare_errors(errors));

	free_tokens(node->token);
	PASS
}

static Token *tokenize_fixture(const char32_t *code) {
	free_errors();
	free_semantic_state();
	Token *token = tokenize(code);
	classify_tokens(token);

	return token;
}
