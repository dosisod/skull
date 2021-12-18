#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/c/assign.h"
#include "skull/codegen/c/shared.h"
#include "skull/codegen/c/types.h"
#include "skull/parse/ast_node.h"
#include "skull/semantic/variable.h"

#include "test/skull/codegen/c/assign.h"
#include "test/skull/semantic/macros.h"
#include "test/testing.h"

static bool top_lvl_var_def_fixture(bool, bool, bool, const char *, const char *);

static void reset_state(void) {
	free(SKULL_STATE_C.globals);
	SKULL_STATE_C.globals = NULL;
}

static bool test_assign(void) {
	AstNode *node = AST_NODE_VAR_ASSIGN(NULL, AST_NODE_CONST_EXPR(NULL));
	SET_EXPR_VALUE_INT(node->var_assign->expr, 1);
	node->var_assign->var = make_variable(TYPE_INT, U"x", false);

	char *str = gen_stmt_var_assign_c(node);

	ASSERT_TRUTHY(str);
	ASSERT_EQUAL(strcmp(str, "x = 1;"), 0);

	free(str);
	free_variable(node->var_assign->var);
	PASS;
}

static bool test_mutable_var_def(void) {
	Token *token = NULL;

	AstNode *node = AST_NODE_VAR_DEF(token, AST_NODE_CONST_EXPR(NULL), true);

	SET_EXPR_VALUE_INT(node->var_def->expr, 1);
	node->var_def->var = make_variable(TYPE_INT, U"x", false);
	node->var_def->var->expr = node->var_def->expr;

	char *str = gen_stmt_var_def_c(node);

	ASSERT_TRUTHY(str);
	ASSERT_EQUAL(strcmp(str, TYPE_INT_C" x = 1;"), 0);

	free(str);
	free_variable(node->var_def->var);
	PASS;
}

static bool test_const_var_def(void) {
	Token *token = NULL;

	AstNode *node = AST_NODE_VAR_DEF(token, AST_NODE_CONST_EXPR(NULL), true);

	SET_EXPR_VALUE_INT(node->var_def->expr, 1);
	node->var_def->var = make_variable(TYPE_INT, U"x", true);
	node->var_def->var->expr = node->var_def->expr;

	char *str = gen_stmt_var_def_c(node);

	ASSERT_TRUTHY(str);
	ASSERT_EQUAL(strcmp(str, "const "TYPE_INT_C" x = 1;"), 0);

	free(str);
	free_variable(node->var_def->var);
	PASS;
}

static bool test_top_lvl_var_def(void) {
	return top_lvl_var_def_fixture(false, false, false, "\nstatic "TYPE_INT_C" x;", "x = 1;");
}

static bool test_top_lvl_constexpr_var_def(void) {
	return top_lvl_var_def_fixture(false, false, true, "\nstatic "TYPE_INT_C" x = 1;", NULL);
}

static bool test_top_lvl_const_var_def(void) {
	return top_lvl_var_def_fixture(true, false, false, "\nstatic "TYPE_INT_C" x;", "x = 1;");
}

static bool test_top_lvl_const_and_constexpr_var_def(void) {
	return top_lvl_var_def_fixture(true, false, true, "\nstatic const "TYPE_INT_C" x = 1;", NULL);
}

static bool test_top_lvl_const_and_export_var_def(void) {
	return top_lvl_var_def_fixture(true, true, false, "\n"TYPE_INT_C" x;", "x = 1;");
}

static bool test_top_lvl_const_and_export_and_constexpr_var_def(void) {
	return top_lvl_var_def_fixture(true, true, true, "\nconst "TYPE_INT_C" x = 1;", NULL);
}

static bool top_lvl_var_def_fixture(
	bool is_const,
	bool is_export,
	bool is_const_expr,
	const char *expected_global,
	const char *expected_literal
) {
	Token *token = NULL;

	AstNode *node = AST_NODE_VAR_DEF(token, AST_NODE_CONST_EXPR(NULL), true);

	SET_EXPR_VALUE_INT(node->var_def->expr, 1);
	node->var_def->var = make_variable(TYPE_INT, U"x", is_const);
	node->var_def->var->is_exported = is_export;
	node->var_def->expr->is_const_expr = is_const_expr;
	node->var_def->var->expr = node->var_def->expr;

	SKULL_STATE_C.indent_lvl = 1;
	char *str = gen_stmt_var_def_c(node);
	SKULL_STATE_C.indent_lvl = 0;

	if (expected_literal) {
		ASSERT_TRUTHY(str);
		ASSERT_EQUAL(strcmp(str, expected_literal), 0);
	}

	ASSERT_TRUTHY(SKULL_STATE_C.globals);
	ASSERT_EQUAL(strcmp(SKULL_STATE_C.globals, expected_global), 0);

	reset_state();

	free(str);
	free_variable(node->var_def->var);
	PASS;
}

void codegen_c_assign_test_self(bool *pass) {
	RUN_ALL(
		test_assign,
		test_mutable_var_def,
		test_const_var_def,
		test_top_lvl_var_def,
		test_top_lvl_constexpr_var_def,
		test_top_lvl_const_var_def,
		test_top_lvl_const_and_constexpr_var_def,
		test_top_lvl_const_and_export_var_def,
		test_top_lvl_const_and_export_and_constexpr_var_def
	)
}
