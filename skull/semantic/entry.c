#include <stdbool.h>

#include "skull/semantic/entry.h"

static bool validate_ast_node(AstNode *);
static bool validate_stmt_return(AstNode *);
bool assert_sane_child(AstNode *);

/*
Validate an entire AST tree starting at `node`.
*/
bool validate_ast_tree(AstNode *node) {
	while (node) {
		if (node->child && !validate_ast_tree(node->child))
			return false;

		if (!validate_ast_node(node)) return false;
		node = node->next;
	}

	return true;
}

/*
Validate a single AST `node`.
*/
static bool validate_ast_node(AstNode *node) {
	if (node->type == AST_NODE_RETURN) {
		return validate_stmt_return(node);
	}

	return true;
}

static bool validate_stmt_return(AstNode *node) {
	return assert_sane_child(node->next);
}
