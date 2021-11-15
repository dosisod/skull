#include <stdlib.h>

#include "skull/codegen/c/assign.h"
#include "skull/codegen/c/flow.h"
#include "skull/codegen/c/shared.h"
#include "skull/common/str.h"

#include "skull/codegen/c/core.h"

static char *gen_node_c(const AstNode *);

void gen_module_c(const AstNode *node) {
	SKULL_STATE_C.tree = gen_tree_c(node);
}

char *gen_tree_c(const AstNode *node) {
	char *generated = NULL;

	while (node) {
		char *current = gen_node_c(node);

		if (current) {
			if (!generated) {
				generated = current;
			}
			else {
				char *old = generated;
				generated = uvsnprintf("%s\n%s", generated, current);
				free(old);
				free(current);
			}
		}

		node = node->next;
	}

	return generated;
}

static char *gen_node_c(const AstNode *node) {
	switch (node->type) {
		case AST_NODE_VAR_DEF: return var_def_to_string(node);
		case AST_NODE_VAR_ASSIGN: return var_assign_to_string(node);
		case AST_NODE_RETURN: return return_node_to_string(node);
		case AST_NODE_NOOP: return noop_to_string(NULL);
		default: return NULL;
	}
}
