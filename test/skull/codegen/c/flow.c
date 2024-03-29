#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/c/flow.h"
#include "skull/codegen/c/shared.h"
#include "skull/semantic/types.h"
#include "test/skull/semantic/macros.h"
#include "test/testing.h"

#include "test/skull/codegen/c/flow.h"

static bool test_return_node(void) {
	AstNode *node = AST_NODE_RETURN(NULL, AST_NODE_CONST_EXPR(NULL));
	SET_EXPR_VALUE_INT(node->expr, 1);

	SkullStateC *state = setup_c_state(NULL);
	char *str = gen_stmt_return_c(node, state);

	ASSERT_TRUTHY(strcmp(str, "return 1;") == 0);

	free(str);
	free_c_state(state);
	PASS;
}

static bool test_return_no_expr(void) {
	AstNode *node = AST_NODE_RETURN(NULL, NULL);
	SkullStateC *state = setup_c_state(NULL);
	char *str = gen_stmt_return_c(node, state);

	ASSERT_TRUTHY(strcmp(str, "return;") == 0);

	free(str);
	free_c_state(state);
	PASS;
}

static bool test_noop(void) {
	char *str = gen_stmt_noop_c(NULL);

	ASSERT_TRUTHY(strcmp(str, "(void)0;") == 0);

	free(str);
	PASS;
}

static bool test_break(void) {
	char *str = gen_stmt_break_c(NULL);

	ASSERT_TRUTHY(strcmp(str, "break;") == 0);

	free(str);
	PASS;
}

static bool test_continue(void) {
	char *str = gen_stmt_continue_c(NULL);

	ASSERT_TRUTHY(strcmp(str, "continue;") == 0);

	free(str);
	PASS;
}

static bool test_if(void) {
	AstNode *node = AST_NODE_IF(NULL, AST_NODE_CONST_EXPR(NULL), AST_NODE_NOOP());
	SET_EXPR_VALUE_BOOL(node->expr, true);

	SkullStateC *state = setup_c_state(NULL);
	char *str = gen_control_if_c(node, state);

	ASSERT_TRUTHY(strcmp(str, "if (1) {\n\t(void)0;\n}") == 0);

	free(str);
	free_c_state(state);
	PASS;
}

static bool test_elif(void) {
	AstNode *node = AST_NODE_ELIF(NULL, AST_NODE_CONST_EXPR(NULL), AST_NODE_NOOP());
	SET_EXPR_VALUE_BOOL(node->expr, true);

	SkullStateC *state = setup_c_state(NULL);
	char *str = gen_control_elif_c(node, state);

	ASSERT_TRUTHY(strcmp(str, "else if (1) {\n\t(void)0;\n}") == 0);

	free(str);
	free_c_state(state);
	PASS;
}

static bool test_else(void) {
	AstNode *node = AST_NODE_ELSE(NULL, AST_NODE_NOOP());

	SkullStateC *state = setup_c_state(NULL);
	char *str = gen_control_else_c(node, state);

	ASSERT_TRUTHY(strcmp(str, "else {\n\t(void)0;\n}") == 0);

	free(str);
	free_c_state(state);
	PASS;
}

static bool test_while(void) {
	AstNode *node = AST_NODE_WHILE(NULL, AST_NODE_CONST_EXPR(NULL), AST_NODE_NOOP());
	SET_EXPR_VALUE_BOOL(node->expr, true);

	SkullStateC *state = setup_c_state(NULL);
	char *str = gen_control_while_c(node, state);

	ASSERT_TRUTHY(strcmp(str, "while (1) {\n\t(void)0;\n}") == 0);

	free(str);
	free_c_state(state);
	PASS;
}

void codegen_c_flow_test_self(bool *pass) {
	RUN_ALL(
		test_return_node,
		test_return_no_expr,
		test_noop,
		test_break,
		test_continue,
		test_if,
		test_elif,
		test_else,
		test_while
	)
}
