#include <stdlib.h>

#include "skull/codegen/c/assign.h"
#include "skull/codegen/c/debug.h"
#include "skull/codegen/c/expr.h"
#include "skull/codegen/c/flow.h"
#include "skull/codegen/c/func.h"
#include "skull/codegen/c/shared.h"
#include "skull/common/str.h"

#include "skull/codegen/c/core.h"

static CStmt gen_node_c(const AstNode *, SkullStateC *);
static char *apply_indentation(char *, SkullStateC *);

void gen_module_c(const AstNode *node, SkullStateC *state) {
	state->tree = gen_tree_c(node, state);
}

CTree gen_tree_c(const AstNode *node, SkullStateC *state) {
	state->indent_lvl++;

	char *generated = NULL;

	while (node) {
		CStmt current = gen_node_c(node, state);

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

	state->indent_lvl--;

	return generated;
}

static CStmt gen_node_c(const AstNode *node, SkullStateC *state) {
	char *str = NULL;

	switch (node->type) {
		case AST_NODE_VAR_DEF: str = gen_stmt_var_def_c(node, state); break;
		case AST_NODE_VAR_ASSIGN:
			str = gen_stmt_var_assign_c(node, state); break;
		case AST_NODE_RETURN: str = gen_stmt_return_c(node, state); break;
		case AST_NODE_NOOP: str = gen_stmt_noop_c(NULL); break;
		case AST_NODE_EXPR: str = gen_stmt_func_call_c(node, state); break;
		case AST_NODE_IF: str = gen_control_if_c(node, state); break;
		case AST_NODE_ELIF: str = gen_control_elif_c(node, state); break;
		case AST_NODE_ELSE: str = gen_control_else_c(node, state); break;
		case AST_NODE_WHILE: str = gen_control_while_c(node, state); break;
		case AST_NODE_UNREACHABLE: str = gen_stmt_unreachable_c(NULL); break;
		case AST_NODE_FUNCTION_PROTO: gen_function_def_c(node, state); break;
		case AST_NODE_NAMESPACE:
			str = gen_control_namespace_c(node, state); break;
		case AST_NODE_BREAK: str = gen_stmt_break_c(node); break;
		case AST_NODE_CONTINUE: str = gen_stmt_continue_c(node); break;
		default: break;
	}

	if (!str) return NULL;

	return apply_debug_info(apply_indentation(str, state), node);
}

static char *apply_indentation(char *str, SkullStateC *state) {
	char *old = str;
	str = uvsnprintf("%s%s", get_indentation(state), old);
	free(old);

	return str;
}
