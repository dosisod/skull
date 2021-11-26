#include <stdlib.h>

#include "skull/codegen/c/assign.h"
#include "skull/codegen/c/expr.h"
#include "skull/codegen/c/flow.h"
#include "skull/codegen/c/func.h"
#include "skull/codegen/c/shared.h"
#include "skull/common/str.h"

#include "skull/codegen/c/core.h"

static CStmt gen_node_c(const AstNode *);
static char *apply_indentation(char *);

void gen_module_c(const AstNode *node) {
	SKULL_STATE_C.tree = gen_tree_c(node);
}

CTree gen_tree_c(const AstNode *node) {
	SKULL_STATE_C.indent_lvl++;

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

	SKULL_STATE_C.indent_lvl--;

	return generated;
}

static CStmt gen_node_c(const AstNode *node) {
	char *str = NULL;

	switch (node->type) {
		case AST_NODE_VAR_DEF: str = gen_stmt_var_def_c(node); break;
		case AST_NODE_VAR_ASSIGN: str = gen_stmt_var_assign_c(node); break;
		case AST_NODE_RETURN: str = gen_stmt_return_c(node); break;
		case AST_NODE_NOOP: str = gen_stmt_noop_c(NULL); break;
		case AST_NODE_EXPR: str = gen_expr_c(node->expr); break;
		case AST_NODE_IF: str = gen_control_if_c(node); break;
		case AST_NODE_ELIF: str = gen_control_elif_c(node); break;
		case AST_NODE_ELSE: str = gen_control_else_c(node); break;
		case AST_NODE_WHILE: str = gen_control_while_c(node); break;
		case AST_NODE_UNREACHABLE: str = gen_stmt_unreachable_c(NULL); break;
		case AST_NODE_FUNCTION_PROTO: gen_function_def_c(node); break;
		default: break;
	}

	if (!str) return NULL;

	return apply_indentation(str);
}

static char *apply_indentation(char *str) {
	char *old = str;
	str = uvsnprintf("%s%s", get_indentation(), old);
	free(old);

	return str;
}
