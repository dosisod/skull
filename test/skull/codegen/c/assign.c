#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/c/assign.h"
#include "skull/codegen/c/shared.h"
#include "skull/codegen/c/types.h"
#include "skull/parse/ast_node.h"
#include "skull/semantic/symbol.h"
#include "skull/semantic/variable.h"

#include "test/skull/codegen/c/assign.h"
#include "test/skull/semantic/macros.h"
#include "test/testing.h"

static bool top_lvl_var_def_fixture(bool, bool, bool, const char *, const char *);

static bool test_assign(void) {
	AstNode *node = AST_NODE_VAR_ASSIGN(NULL, AST_NODE_CONST_EXPR(NULL));
	SET_EXPR_VALUE_INT(node->var_assign->expr, 1);
	node->var_assign->symbol->var = make_variable(&TYPE_INT, U"x", false);

	SkullStateC *state = setup_c_state(NULL);
	char *str = gen_stmt_var_assign_c(node, state);

	ASSERT_TRUTHY(str);
	ASSERT_EQUAL(strcmp(str, "x = 1;"), 0);

	free(str);
	free_variable(node->var_assign->symbol->var);
	free_c_state(state);
	PASS;
}

static bool test_mutable_var_def(void) {
	Token *token = NULL;

	AstNode *node = AST_NODE_VAR_DEF(token, AST_NODE_CONST_EXPR(NULL), true);

	SET_EXPR_VALUE_INT(node->var_def->expr, 1);
	node->var_def->symbol->var = make_variable(&TYPE_INT, U"x", false);
	node->var_def->symbol->var->expr = node->var_def->expr;

	SkullStateC *state = setup_c_state(NULL);
	char *str = gen_stmt_var_def_c(node, state);

	ASSERT_TRUTHY(str);
	ASSERT_EQUAL(strcmp(str, TYPE_INT_C" x = 1;"), 0);

	free(str);
	free_variable(node->var_def->symbol->var);
	free_c_state(state);
	PASS;
}

static bool test_const_var_def(void) {
	Token *token = NULL;

	AstNode *node = AST_NODE_VAR_DEF(token, AST_NODE_CONST_EXPR(NULL), true);

	SET_EXPR_VALUE_INT(node->var_def->expr, 1);
	node->var_def->symbol->var = make_variable(&TYPE_INT, U"x", true);
	node->var_def->symbol->var->expr = node->var_def->expr;

	SkullStateC *state = setup_c_state(NULL);
	char *str = gen_stmt_var_def_c(node, state);

	ASSERT_TRUTHY(str);
	ASSERT_EQUAL(strcmp(str, "const "TYPE_INT_C" x = 1;"), 0);

	free(str);
	free_variable(node->var_def->symbol->var);
	free_c_state(state);
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
	return top_lvl_var_def_fixture(true, true, false, "\n"TYPE_INT_C" x __asm__(\"x\");", "x = 1;");
}

static bool test_top_lvl_const_and_export_and_constexpr_var_def(void) {
	return top_lvl_var_def_fixture(true, true, true, "\nconst "TYPE_INT_C" x __asm__(\"x\") = 1;", NULL);
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
	node->var_def->symbol->var = make_variable(&TYPE_INT, U"x", is_const);
	node->var_def->symbol->var->is_exported = is_export;
	node->var_def->expr->is_const_expr = is_const_expr;
	node->var_def->symbol->var->expr = node->var_def->expr;

	SkullStateC *state = setup_c_state(NULL);

	state->indent_lvl = 1;
	char *str = gen_stmt_var_def_c(node, state);
	state->indent_lvl = 0;

	if (expected_literal) {
		ASSERT_TRUTHY(str);
		ASSERT_EQUAL(strcmp(str, expected_literal), 0);
	}

	ASSERT_TRUTHY(state->globals);
	ASSERT_EQUAL(strcmp(state->globals, expected_global), 0);

	free_c_state(state);

	free(str);
	free_variable(node->var_def->symbol->var);
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
