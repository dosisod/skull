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

static void reset_state(void) {
	free(SKULL_STATE_C.globals);
	SKULL_STATE_C.globals = NULL;
}

static bool test_assign(void) {
	AstNode *node = AST_NODE_VAR_ASSIGN(NULL, AST_SIMPLE_EXPR(NULL));
	SET_EXPR_VALUE_INT(node->var_assign->expr_node->expr, 1);
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

	AstNode *node = AST_NODE_VAR_DEF(token, AST_SIMPLE_EXPR(NULL), true);

	SET_EXPR_VALUE_INT(node->var_def->expr_node->expr, 1);
	node->var_def->var = make_variable(TYPE_INT, U"x", false);

	char *str = gen_stmt_var_def_c(node);

	ASSERT_TRUTHY(str);
	ASSERT_EQUAL(strcmp(str, TYPE_INT_C" x = 1;"), 0);

	free(str);
	free_variable(node->var_def->var);
	PASS;
}

static bool test_const_var_def(void) {
	Token *token = NULL;

	AstNode *node = AST_NODE_VAR_DEF(token, AST_SIMPLE_EXPR(NULL), true);

	SET_EXPR_VALUE_INT(node->var_def->expr_node->expr, 1);
	node->var_def->var = make_variable(TYPE_INT, U"x", true);

	char *str = gen_stmt_var_def_c(node);

	ASSERT_TRUTHY(str);
	ASSERT_EQUAL(strcmp(str, "const "TYPE_INT_C" x = 1;"), 0);

	free(str);
	free_variable(node->var_def->var);
	PASS;
}

static bool test_top_lvl_var_def(void) {
	Token *token = NULL;

	AstNode *node = AST_NODE_VAR_DEF(token, AST_SIMPLE_EXPR(NULL), true);

	SET_EXPR_VALUE_INT(node->var_def->expr_node->expr, 1);
	node->var_def->var = make_variable(TYPE_INT, U"x", true);

	SKULL_STATE_C.indent_lvl = 1;
	char *str = gen_stmt_var_def_c(node);
	SKULL_STATE_C.indent_lvl = 0;

	ASSERT_TRUTHY(str);
	ASSERT_TRUTHY(SKULL_STATE_C.globals);
	ASSERT_EQUAL(strcmp(SKULL_STATE_C.globals, "\nstatic "TYPE_INT_C" x;"), 0);
	ASSERT_EQUAL(strcmp(str, "x = 1;"), 0);

	reset_state();

	free(str);
	free_variable(node->var_def->var);
	PASS;
}

static bool test_top_lvl_export_var_def(void) {
	Token *token = NULL;

	AstNode *node = AST_NODE_VAR_DEF(token, AST_SIMPLE_EXPR(NULL), true);

	SET_EXPR_VALUE_INT(node->var_def->expr_node->expr, 1);
	node->var_def->var = make_variable(TYPE_INT, U"x", true);
	node->var_def->var->is_exported = true;

	SKULL_STATE_C.indent_lvl = 1;
	char *str = gen_stmt_var_def_c(node);
	SKULL_STATE_C.indent_lvl = 0;

	ASSERT_TRUTHY(str);
	ASSERT_TRUTHY(SKULL_STATE_C.globals);
	ASSERT_EQUAL(strcmp(SKULL_STATE_C.globals, "\n"TYPE_INT_C" x;"), 0);
	ASSERT_EQUAL(strcmp(str, "x = 1;"), 0);

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
		test_top_lvl_export_var_def
	)
}
