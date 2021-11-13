#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/c/core.h"
#include "test/skull/semantic/macros.h"
#include "test/testing.h"

bool test_parse_single_node() {
	AstNode *node = AST_NODE_NOOP();

	char *tree = gen_tree_c(node);

	ASSERT_TRUTHY(strcmp(tree, "(void)0;") == 0);

	free(tree);
	PASS;
}

bool test_parse_multiple_nodes() {
	AstNode *node = AST_NODE_NOOP();
	node->next = AST_NODE_NOOP();

	char *tree = gen_tree_c(node);

	ASSERT_TRUTHY(strcmp(tree, "(void)0;\n(void)0;") == 0);

	free(tree);
	PASS;
}

void codegen_c_core_test_self(bool *pass) {
	RUN_ALL(
		test_parse_single_node,
		test_parse_multiple_nodes
	)
}
