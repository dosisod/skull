#include <stdbool.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/codegen/scope.h"

#include "skull/semantic/entry.h"

static bool validate_ast_node(AstNode *);
static bool validate_stmt_return(AstNode *);
static bool validate_stmt_func_decl(AstNode *);
static bool validate_stmt_type_alias(AstNode *);
bool assert_sane_child(AstNode *);

/*
Validate an entire AST tree starting at `node`.
*/
bool validate_ast_tree(AstNode *node) {
	while (node) {
		if (node->child) {
			Scope *scope_copy = NULL;
			make_sub_scope(&SKULL_STATE.scope, &scope_copy);
			const bool is_valid = validate_ast_tree(node->child);
			restore_sub_scope(&SKULL_STATE.scope, &scope_copy);

			if (!is_valid) return false;
		}

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
	if (node->type == AST_NODE_TYPE_ALIAS) {
		return validate_stmt_type_alias(node);
	}

	return true;
}

static bool validate_stmt_return(AstNode *node) {
	return assert_sane_child(node->next);
}

static bool validate_stmt_func_decl(AstNode *node) {
	const bool is_external = node->func_proto->is_external;
	const bool is_export = node->func_proto->is_export;
	const Token *const func_name_token = node->func_proto->name_tok;

	if ((is_export || is_external) &&
		SKULL_STATE.scope &&
		SKULL_STATE.scope->sub_scope
	) {
		FMT_ERROR(ERR_NO_NESTED, { .tok = func_name_token });
		return false;
	}

	char *func_name = token_mbs_str(func_name_token);

	if (is_export && strcmp(func_name, "main") == 0) {
		FMT_ERROR(ERR_MAIN_RESERVED, { .loc = &func_name_token->location });

		free(func_name);
		return false;
	}

	free(func_name);
	return true;
}

static bool validate_stmt_type_alias(AstNode *node) {
	const Token *const token = node->token;

	if (SKULL_STATE.scope && SKULL_STATE.scope->sub_scope) {
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
