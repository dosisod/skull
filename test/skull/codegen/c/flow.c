#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/c/flow.h"
#include "skull/semantic/types.h"
#include "test/skull/semantic/macros.h"
#include "test/testing.h"

#include "test/skull/codegen/c/flow.h"

bool test_return_node_to_string() {
	AstNode *node = AST_NODE_RETURN(
		NULL,
		AST_NODE_EXPR(NULL, AST_NODE_CONST_EXPR(NULL))
	);
	node->expr_node->expr->type = TYPE_INT;
	node->expr_node->expr->value._int = 1;
	node->expr_node->expr->oper = EXPR_CONST;

	char *str = return_node_to_string(node);

	ASSERT_TRUTHY(strcmp(str, "return 1;") == 0);

	free(str);
	PASS;
}

bool test_return_no_expr_to_string() {
	AstNode *node = AST_NODE_RETURN(NULL, NULL);
	char *str = return_node_to_string(node);

	ASSERT_TRUTHY(strcmp(str, "return;") == 0);

	free(str);
	PASS;
}

bool test_noop_to_string() {
	char *str = noop_to_string(NULL);

	ASSERT_TRUTHY(strcmp(str, "(void)0;") == 0);

	free(str);
	PASS;
}

bool test_if_to_string() {
	AstNode *node = AST_NODE_IF(NULL, AST_SIMPLE_EXPR(NULL), NULL);
	node->expr_node->expr->type = TYPE_BOOL;
	node->expr_node->expr->value._bool = true;

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
