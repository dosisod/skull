#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/c/flow.h"
#include "skull/semantic/types.h"
#include "test/skull/semantic/macros.h"
#include "test/testing.h"

#include "test/skull/codegen/c/flow.h"

bool test_return_node_to_string(void) {
	AstNode *node = AST_NODE_RETURN(
		NULL,
		AST_NODE_EXPR(NULL, AST_NODE_CONST_EXPR(NULL))
	);
	SET_EXPR_VALUE_INT(node->expr_node->expr, 1);

	char *str = return_node_to_string(node);

	ASSERT_TRUTHY(strcmp(str, "return 1;") == 0);

	free(str);
	PASS;
}

bool test_return_no_expr_to_string(void) {
	AstNode *node = AST_NODE_RETURN(NULL, NULL);
	char *str = return_node_to_string(node);

	ASSERT_TRUTHY(strcmp(str, "return;") == 0);

	free(str);
	PASS;
}

bool test_noop_to_string(void) {
	char *str = noop_to_string(NULL);

	ASSERT_TRUTHY(strcmp(str, "(void)0;") == 0);

	free(str);
	PASS;
}

bool test_if_to_string(void) {
	AstNode *node = AST_NODE_IF(NULL, AST_SIMPLE_EXPR(NULL), NULL);
	SET_EXPR_VALUE_BOOL(node->expr_node->expr, true);

	char *str = if_to_string(node);

	ASSERT_TRUTHY(strcmp(str, "if (1) {}") == 0);

	free(str);
	PASS;
}

void codegen_c_flow_test_self(bool *pass) {
	RUN_ALL(
		test_return_node_to_string,
		test_return_no_expr_to_string,
		test_noop_to_string,
		test_if_to_string
	)
}
