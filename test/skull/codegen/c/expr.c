#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/c/expr.h"
#include "skull/common/str.h"
#include "skull/parse/classify.h"
#include "skull/parse/token.h"
#include "skull/semantic/types.h"
#include "skull/semantic/variable.h"
#include "test/skull/semantic/macros.h"
#include "test/testing.h"

bool expr_to_string_fixture(const AstNodeExpr *expr, const char *expected) {
	char *out = expr_node_to_string(expr);

	const bool pass = out && strcmp(out, expected) == 0;

	free(out);
	return pass;
}

bool test_int_expr_to_string() {
	ASSERT_TRUTHY(
		expr_to_string_fixture(
			AST_NODE_CONST_INT(1234),
			"1234"
		)
	);

	char *expected = uvsnprintf("%liL", INT_MAX + 1L);

	ASSERT_TRUTHY(
		expr_to_string_fixture(
			AST_NODE_CONST_INT(INT_MAX + 1L),
			expected
		)
	);

	free(expected);
	PASS;
}

bool test_float_expr_to_string() {
	ASSERT_TRUTHY(
		expr_to_string_fixture(
			AST_NODE_CONST_FLOAT(3.14),
			"0x1.91eb851eb851fp+1"
		)
	);

	ASSERT_TRUTHY(
		expr_to_string_fixture(AST_NODE_CONST_FLOAT(0), "0.0")
	);

	PASS;
}

bool test_bool_expr_to_string() {
	ASSERT_TRUTHY(
		expr_to_string_fixture(AST_NODE_CONST_BOOL(true), "1")
	);

	ASSERT_TRUTHY(
		expr_to_string_fixture(AST_NODE_CONST_BOOL(false), "0")
	);

	PASS;
}

bool test_rune_expr_to_string() {
	ASSERT_TRUTHY(
		expr_to_string_fixture(AST_NODE_CONST_RUNE('A'), "0x41")
	);

	PASS;
}

bool test_identifier_expr_to_string() {
	Variable *var = make_variable(TYPE_INT, U"x", false);

	ASSERT_TRUTHY(expr_to_string_fixture(AST_NODE_EXPR_VAR(var), "x"));

	free_variable(var);

	PASS;
}

bool test_binary_expr_to_string() {
	ExprType *opers = (ExprType[]){
		EXPR_ADD,
		EXPR_SUB,
		EXPR_MULT,
		EXPR_DIV,
		EXPR_MOD,
		EXPR_LSHIFT,
		EXPR_RSHIFT,
		EXPR_IS,
		EXPR_ISNT,
		EXPR_LESS_THAN,
		EXPR_GTR_THAN,
		EXPR_LESS_THAN_EQ,
		EXPR_GTR_THAN_EQ,
		EXPR_AND,
		EXPR_OR,
		EXPR_XOR,
		EXPR_UNKNOWN
	};

	// Not all of these expressions are valid. What is being tested is that
	// the operators are converted to the proper C equivalent.
	const char **expected = (const char*[]){
		"(1 + 1)",
		"(1 - 1)",
		"(1 * 1)",
		"(1 / 1)",
		"(1 % 1)",
		"(1 << 1)",
		"(1 >> 1)",
		"(1 == 1)",
		"(1 != 1)",
		"(1 < 1)",
		"(1 > 1)",
		"(1 <= 1)",
		"(1 >= 1)",
		"(1 && 1)",
		"(1 || 1)",
		"(1 ^ 1)",
		NULL
	};

	bool pass = true;

	do {
		Token *token = tokenize(U"1 x 1");
		classify_tokens(token);
		AstNode *node = AST_SIMPLE_BINARY_EXPR(token, *opers);
		SET_EXPR_VALUE_INT(node->expr->lhs.expr, 1);
		SET_EXPR_VALUE_INT(node->expr->rhs, 1);

		char *expr_str = expr_node_to_string(node->expr);

		pass &= expr_str && strcmp(*expected, expr_str) == 0;

		free(expr_str);
		free_tokens(token);
		++opers;
		++expected;
	} while (*opers && *expected);

	return pass;
}

bool test_int_pow_expr_to_string() {
	AstNodeExpr *expr = AST_NODE_BINARY_EXPR(
		AST_NODE_CONST_EXPR(NULL),
		EXPR_POW,
		AST_NODE_CONST_EXPR(NULL)
	);
	SET_EXPR_VALUE_INT(expr->lhs.expr, 1);
	SET_EXPR_VALUE_INT(expr->rhs, 2);
	expr->type = TYPE_INT;

	char *expr_str = expr_node_to_string(expr);

	ASSERT_TRUTHY(strcmp(expr_str, "_int_pow(1, 2)") == 0);

	free(expr_str);
	PASS;
}

bool test_float_pow_expr_to_string() {
	AstNodeExpr *expr = AST_NODE_BINARY_EXPR(
		AST_NODE_CONST_EXPR(NULL),
		EXPR_POW,
		AST_NODE_CONST_EXPR(NULL)
	);
	SET_EXPR_VALUE_FLOAT(expr->lhs.expr, 1.0);
	SET_EXPR_VALUE_FLOAT(expr->rhs, 2.0);
	expr->type = TYPE_FLOAT;

	char *expr_str = expr_node_to_string(expr);

	const char *expected = "_float_pow(0x1p+0, 0x1p+1)";
	ASSERT_TRUTHY(strcmp(expr_str, expected) == 0);

	free(expr_str);
	PASS;
}

bool test_unary_negation_to_string() {
	AstNodeExpr *expr = AST_NODE_BINARY_EXPR(
		NULL,
		EXPR_UNARY_NEG,
		AST_NODE_CONST_EXPR(NULL)
	);
	SET_EXPR_VALUE_INT(expr->rhs, 1);

	char *expr_str = expr_node_to_string(expr);

	const char *expected = "-(1)";
	ASSERT_TRUTHY(strcmp(expr_str, expected) == 0);

	free(expr_str);
	PASS;
}

bool test_unary_not_to_string() {
	AstNodeExpr *expr = AST_NODE_BINARY_EXPR(
		NULL,
		EXPR_NOT,
		AST_NODE_CONST_EXPR(NULL)
	);
	SET_EXPR_VALUE_INT(expr->rhs, 1);

	char *expr_str = expr_node_to_string(expr);

	const char *expected = "!1";
	ASSERT_TRUTHY(strcmp(expr_str, expected) == 0);

	free(expr_str);
	PASS;
}

void codegen_c_expr_test_self(bool *pass) {
	RUN_ALL(
		test_int_expr_to_string,
		test_float_expr_to_string,
		test_bool_expr_to_string,
		test_rune_expr_to_string,
		test_identifier_expr_to_string,
		test_binary_expr_to_string,
		test_int_pow_expr_to_string,
		test_float_pow_expr_to_string,
		test_unary_negation_to_string,
		test_unary_not_to_string
	)
}
