#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/c/core.h"
#include "skull/codegen/c/shared.h"
#include "test/skull/semantic/macros.h"
#include "test/testing.h"

static bool test_parse_single_node(void) {
	AstNode *node = AST_NODE_NOOP();

	SkullStateC *state = setup_c_state();
	char *tree = gen_tree_c(node, state);

	ASSERT_TRUTHY(strcmp(tree, "\t(void)0;") == 0);

	free(tree);
	free_c_state(state);
	PASS;
}

static bool test_parse_multiple_nodes(void) {
	AstNode *node = AST_NODE_NOOP();
	node->next = AST_NODE_NOOP();

	SkullStateC *state = setup_c_state();
	char *tree = gen_tree_c(node, state);

	ASSERT_TRUTHY(strcmp(tree, "\t(void)0;\n\t(void)0;") == 0);

	free(tree);
	free_c_state(state);
	PASS;
}

void codegen_c_core_test_self(bool *pass) {
	RUN_ALL(
		test_parse_single_node,
		test_parse_multiple_nodes
	)
}
