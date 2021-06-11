#include <stdbool.h>
#include <string.h>

#include "skull/codegen/scope.h"
#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/semantic/assign.h"
#include "skull/semantic/func.h"

#include "skull/semantic/entry.h"


static bool _validate_ast_tree(AstNode *);
static bool validate_ast_node(AstNode *);
static bool validate_stmt_return(AstNode *);
static bool validate_stmt_type_alias(AstNode *);
bool assert_sane_child(AstNode *);

/*
Validate an entire AST tree starting at `node`.
*/
bool validate_ast_tree(AstNode *node) {
	const bool pass = _validate_ast_tree(node);
	reset_scope_head();

	return pass;
}

/*
Validate an entire AST tree starting at `node`.
*/
static bool _validate_ast_tree(AstNode *node) {
	while (node) {
		if (!validate_ast_node(node)) return false;

		if (node->child) {
			make_child_scope();
			const bool is_valid = _validate_ast_tree(node->child);
			restore_parent_scope();

			if (!is_valid) return false;
			make_adjacent_scope();
		}

		node = node->next;
	}

	reset_scope_head();
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
	if (node->type == AST_NODE_TYPE_ALIAS) {
		return validate_stmt_type_alias(node);
	}
	if (node->type == AST_NODE_VAR_DEF) {
		return validate_stmt_var_def(node);
	}

	return true;
}

static bool validate_stmt_return(AstNode *node) {
	return assert_sane_child(node->next);
}

static bool validate_stmt_type_alias(AstNode *node) {
	const Token *const token = node->token;

	if (!is_top_lvl_scope()) {
		FMT_ERROR(ERR_TOP_LVL_ALIAS_ONLY, { .loc = &token->location });
		return false;
	}

	char *type_name = token_mbs_str(token->next->next);
	char *alias = token_mbs_str(token);

	const bool added = state_add_alias(find_type(type_name), alias);
	free(type_name);

	if (added) return true;

	FMT_ERROR(ERR_ALIAS_ALREADY_DEFINED, {
		.loc = &token->location,
		.real = alias
	});

	return false;
}
