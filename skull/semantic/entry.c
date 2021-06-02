#include <stdbool.h>
#include <string.h>

#include "skull/common/errors.h"

#include "skull/semantic/entry.h"

static bool validate_ast_node(AstNode *);
static bool validate_stmt_return(AstNode *);
static bool validate_stmt_func_decl(AstNode *);
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
	if (node->type == AST_NODE_FUNCTION_PROTO) {
		return validate_stmt_func_decl(node);
	}

	return true;
}

static bool validate_stmt_return(AstNode *node) {
	return assert_sane_child(node->next);
}

static bool validate_stmt_func_decl(AstNode *node) {
	const bool is_export = node->func_proto->is_export;

	const Token *const func_name_token = node->func_proto->name_tok;
	char *func_name = token_mbs_str(func_name_token);

	if (is_export && strcmp(func_name, "main") == 0) {
		FMT_ERROR(ERR_MAIN_RESERVED, { .loc = &func_name_token->location });

		free(func_name);
		return false;
	}

	free(func_name);
	return true;
}
