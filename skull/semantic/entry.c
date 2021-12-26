#include <stdbool.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/semantic/assign.h"
#include "skull/semantic/expr.h"
#include "skull/semantic/flow.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/symbol.h"
#include "skull/semantic/unused.h"

#include "skull/semantic/entry.h"


static bool validate_ast_tree_(const AstNode *);
static bool validate_ast_node(const AstNode *);
static bool post_validate_ast_node(const AstNode *);

/*
Validate an entire AST tree starting at `node`.
*/
bool validate_ast_tree(const AstNode *node) {
	const bool pass = validate_ast_tree_(node);
	reset_scope_head();

	return pass && check_unused_symbols(node);
}

/*
Validate an entire AST tree starting at `node`.
*/
static bool validate_ast_tree_(const AstNode *node) {
	while (node) {
		if (!validate_ast_node(node)) return false;

		if (node->child) {
			make_child_scope();
			const bool is_valid = validate_ast_tree_(node->child);
			restore_parent_scope();

			if (!is_valid) return false;
			make_adjacent_scope();
		}

		if (!post_validate_ast_node(node)) return false;

		node = node->next;
	}

	reset_scope_head();
	return true;
}

static bool post_validate_ast_node(const AstNode *node) {
	if (node->type == AST_NODE_FUNCTION_PROTO) {
		return post_validate_stmt_func_decl(node);
	}
	if (node->type == AST_NODE_WHILE) {
		SEMANTIC_STATE.while_loop_depth--;
	}

	return true;
}

/*
Validate a single AST `node`.
*/
static bool validate_ast_node(const AstNode *node) {
	if (!assert_sane_child(node)) return false;

	switch (node->type) {
		case AST_NODE_RETURN: return validate_stmt_return(node);
		case AST_NODE_UNREACHABLE: return validate_stmt_unreachable(node);
		case AST_NODE_FUNCTION_PROTO: return validate_stmt_func_decl(node);
		case AST_NODE_VAR_DEF: return validate_stmt_var_def(node);
		case AST_NODE_VAR_ASSIGN: return validate_stmt_var_assign(node);
		case AST_NODE_EXPR: return validate_expr_func(node);
		case AST_NODE_IF: return validate_control_if(node);
		case AST_NODE_ELSE: return validate_control_else(node);
		case AST_NODE_ELIF: return validate_control_elif(node);
		case AST_NODE_WHILE: return validate_control_while(node);
		case AST_NODE_BREAK: return validate_control_break(node);
		case AST_NODE_CONTINUE: return validate_control_continue(node);
		default: return true;
	}
}
