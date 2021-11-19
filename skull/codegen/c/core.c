#include <stdlib.h>

#include "skull/codegen/c/assign.h"
#include "skull/codegen/c/expr.h"
#include "skull/codegen/c/flow.h"
#include "skull/codegen/c/shared.h"
#include "skull/common/str.h"

#include "skull/codegen/c/core.h"

static CStmt gen_node_c(const AstNode *);

void gen_module_c(const AstNode *node) {
	SKULL_STATE_C.tree = gen_tree_c(node);
}

CTree gen_tree_c(const AstNode *node) {
	char *generated = NULL;

	while (node) {
		CStmt current = gen_node_c(node);

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

static CStmt gen_node_c(const AstNode *node) {
	switch (node->type) {
		case AST_NODE_VAR_DEF: return gen_stmt_var_def_c(node);
		case AST_NODE_VAR_ASSIGN: return gen_stmt_var_assign_c(node);
		case AST_NODE_RETURN: return gen_stmt_return_c(node);
		case AST_NODE_NOOP: return gen_stmt_noop_c(NULL);
		case AST_NODE_EXPR: return gen_expr_c(node->expr);
		default: return NULL;
	}
}
