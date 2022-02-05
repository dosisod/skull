#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/c/expr.h"
#include "skull/codegen/c/shared.h"
#include "skull/common/str.h"
#include "skull/parse/classify.h"
#include "skull/parse/token.h"
#include "skull/semantic/types.h"
#include "skull/semantic/variable.h"
#include "test/skull/semantic/macros.h"
#include "test/testing.h"

static bool expr_fixture(const AstNodeExpr *expr, const char *expected) {
	char *out = gen_expr_c(expr, NULL);

	const bool pass = out && strcmp(out, expected) == 0;

	free(out);
	return pass;
}

static bool test_int_expr(void) {
	ASSERT_TRUTHY(
		expr_fixture(
			AST_NODE_CONST_INT(1234),
			"1234"
		)
	);

	char *expected = uvsnprintf("%liL", INT_MAX + 1L);

	ASSERT_TRUTHY(
		expr_fixture(
			AST_NODE_CONST_INT(INT_MAX + 1L),
			expected
		)
	);

	free(expected);
	PASS;
}

static bool test_float_expr(void) {
	ASSERT_TRUTHY(
		expr_fixture(
			AST_NODE_CONST_FLOAT(3.14),
			"0x1.91eb851eb851fp+1"
		)
	);

	ASSERT_TRUTHY(
		expr_fixture(AST_NODE_CONST_FLOAT(0), "0.0")
	);

	PASS;
}

static bool test_bool_expr(void) {
	ASSERT_TRUTHY(
		expr_fixture(AST_NODE_CONST_BOOL(true), "1")
	);

	ASSERT_TRUTHY(
		expr_fixture(AST_NODE_CONST_BOOL(false), "0")
	);

	PASS;
}

static bool test_rune_expr(void) {
	ASSERT_TRUTHY(
		expr_fixture(AST_NODE_CONST_RUNE('A'), "0x41")
	);

	PASS;
}

static bool test_str_expr(void) {
	ASSERT_TRUTHY(
		expr_fixture(AST_NODE_CONST_STR((char[]){"hello"}), "\"hello\"")
	);

	PASS;
}

static bool test_identifier_expr(void) {
	Variable *var = make_variable(&TYPE_INT, U"x", false);

	ASSERT_TRUTHY(expr_fixture(AST_NODE_EXPR_VAR(var), "x"));

	free_variable(var);

	PASS;
}

static bool test_binary_expr(void) {
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

		char *expr_str = gen_expr_c(node->expr, NULL);

		pass &= expr_str && strcmp(*expected, expr_str) == 0;

		free(expr_str);
		free_tokens(token);
		++opers;
		++expected;
	} while (*opers && *expected);

	return pass;
}

static bool test_int_pow_expr(void) {
	AstNodeExpr *expr = AST_NODE_BINARY_EXPR(
		AST_NODE_CONST_EXPR(NULL),
		EXPR_POW,
		AST_NODE_CONST_EXPR(NULL)
	);
	SET_EXPR_VALUE_INT(expr->lhs.expr, 1);
	SET_EXPR_VALUE_INT(expr->rhs, 2);
	expr->type = &TYPE_INT;

	SkullStateC *state = setup_c_state();
	char *expr_str = gen_expr_c(expr, state);

	ASSERT_TRUTHY(strcmp(expr_str, "_int_pow(1, 2)") == 0);
	ASSERT_TRUTHY(state->called_int_pow);

	free(expr_str);
	PASS;
}

static bool test_float_pow_expr(void) {
	AstNodeExpr *expr = AST_NODE_BINARY_EXPR(
		AST_NODE_CONST_EXPR(NULL),
		EXPR_POW,
		AST_NODE_CONST_EXPR(NULL)
	);
	SET_EXPR_VALUE_FLOAT(expr->lhs.expr, 1.0);
	SET_EXPR_VALUE_FLOAT(expr->rhs, 2.0);
	expr->type = &TYPE_FLOAT;

	SkullStateC *state = setup_c_state();
	char *expr_str = gen_expr_c(expr, state);

	const char *expected = "_float_pow(0x1p+0, 0x1p+1)";
	ASSERT_TRUTHY(strcmp(expr_str, expected) == 0);
	ASSERT_TRUTHY(state->called_float_pow);

	free(expr_str);
	PASS;
}

static bool test_str_is(void) {
	AstNodeExpr *expr = AST_NODE_BINARY_EXPR(
		AST_NODE_CONST_EXPR(NULL),
		EXPR_IS,
		AST_NODE_CONST_EXPR(NULL)
	);
	SET_EXPR_VALUE_STR(expr->lhs.expr, "x");
	SET_EXPR_VALUE_STR(expr->rhs, "x");
	expr->type = &TYPE_BOOL;

	SkullStateC *state = setup_c_state();
	char *expr_str = gen_expr_c(expr, state);

	const char *expected = "_strcmp(\"x\", \"x\")";
	ASSERT_TRUTHY(strcmp(expr_str, expected) == 0);
	ASSERT_TRUTHY(state->called_strcmp);

	free(expr_str);
	PASS;
}

static bool test_str_isnt(void) {
	AstNodeExpr *expr = AST_NODE_BINARY_EXPR(
		AST_NODE_CONST_EXPR(NULL),
		EXPR_ISNT,
		AST_NODE_CONST_EXPR(NULL)
	);
	SET_EXPR_VALUE_STR(expr->lhs.expr, "x");
	SET_EXPR_VALUE_STR(expr->rhs, "x");
	expr->type = &TYPE_BOOL;

	SkullStateC *state = setup_c_state();
	char *expr_str = gen_expr_c(expr, state);

	const char *expected = "!_strcmp(\"x\", \"x\")";
	ASSERT_TRUTHY(strcmp(expr_str, expected) == 0);
	ASSERT_TRUTHY(state->called_strcmp);

	free(expr_str);
	PASS;
}

static bool test_unary_negation(void) {
	AstNodeExpr *expr = AST_NODE_BINARY_EXPR(
		NULL,
		EXPR_UNARY_NEG,
		AST_NODE_CONST_EXPR(NULL)
	);
	SET_EXPR_VALUE_INT(expr->rhs, 1);

	char *expr_str = gen_expr_c(expr, NULL);

	const char *expected = "-(1)";
	ASSERT_TRUTHY(strcmp(expr_str, expected) == 0);

	free(expr_str);
	PASS;
}

static bool test_unary_not(void) {
	AstNodeExpr *expr = AST_NODE_BINARY_EXPR(
		NULL,
		EXPR_NOT,
		AST_NODE_CONST_EXPR(NULL)
	);
	SET_EXPR_VALUE_INT(expr->rhs, 1);

	char *expr_str = gen_expr_c(expr, NULL);

	ASSERT_TRUTHY(strcmp(expr_str, "!1") == 0);

	free(expr_str);
	PASS;
}

static bool test_func_call_no_args(void) {
	AstNodeExpr *expr = AST_NODE_FUNC_EXPR(NULL);
	char func_name[] = "f";
	expr->lhs.func_call->func_decl->name = func_name;

	char *expr_str = gen_expr_c(expr, NULL);

	ASSERT_TRUTHY(strcmp(expr_str, "f()") == 0);

	free(expr_str);
	PASS;
}

static bool test_func_call_single_arg(void) {
	AstNodeExpr *expr = AST_NODE_FUNC_EXPR(NULL);

	AST_NODE_FUNC_ADD_PARAM(expr, AST_NODE_EXPR(NULL, AST_NODE_CONST_EXPR(NULL)));

	expr->lhs.func_call->func_decl->name = (char[]){"f"};
	SET_EXPR_VALUE_INT(expr->lhs.func_call->params->expr, 1);

	char *expr_str = gen_expr_c(expr, NULL);

	ASSERT_TRUTHY(strcmp(expr_str, "f(1)") == 0);

	free(expr_str);
	PASS;
}

static bool test_func_call_two_args(void) {
	AstNodeExpr *expr = AST_NODE_FUNC_EXPR(NULL);

	AST_NODE_FUNC_ADD_PARAM(expr, AST_NODE_EXPR(NULL, AST_NODE_CONST_EXPR(NULL)));
	AST_NODE_FUNC_ADD_PARAM(expr, AST_NODE_EXPR(NULL, AST_NODE_CONST_EXPR(NULL)));

	expr->lhs.func_call->func_decl->name = (char[]){"f"};
	SET_EXPR_VALUE_INT(expr->lhs.func_call->params->expr, 1);
	SET_EXPR_VALUE_INT(expr->lhs.func_call->params->next->expr, 2);

	char *expr_str = gen_expr_c(expr, NULL);

	ASSERT_TRUTHY(strcmp(expr_str, "f(1, 2)") == 0);

	free(expr_str);
	PASS;
}

static bool test_func_call_many_args(void) {
	AstNodeExpr *expr = AST_NODE_FUNC_EXPR(NULL);

	AST_NODE_FUNC_ADD_PARAM(expr, AST_NODE_EXPR(NULL, AST_NODE_CONST_EXPR(NULL)));
	AST_NODE_FUNC_ADD_PARAM(expr, AST_NODE_EXPR(NULL, AST_NODE_CONST_EXPR(NULL)));
	AST_NODE_FUNC_ADD_PARAM(expr, AST_NODE_EXPR(NULL, AST_NODE_CONST_EXPR(NULL)));

	expr->lhs.func_call->func_decl->name = (char[]){"f"};
	SET_EXPR_VALUE_INT(expr->lhs.func_call->params->expr, 1);
	SET_EXPR_VALUE_INT(expr->lhs.func_call->params->next->expr, 2);
	SET_EXPR_VALUE_INT(expr->lhs.func_call->params->next->next->expr, 3);

	char *expr_str = gen_expr_c(expr, NULL);

	ASSERT_TRUTHY(strcmp(expr_str, "f(1, 2, 3)") == 0);

	free(expr_str);
	PASS;
}

void codegen_c_expr_test_self(bool *pass) {
	RUN_ALL(
		test_int_expr,
		test_float_expr,
		test_bool_expr,
		test_rune_expr,
		test_str_expr,
		test_identifier_expr,
		test_binary_expr,
		test_int_pow_expr,
		test_float_pow_expr,
		test_str_is,
		test_str_isnt,
		test_unary_negation,
		test_unary_not,
		test_func_call_no_args,
		test_func_call_single_arg,
		test_func_call_two_args,
		test_func_call_many_args
	)
}
