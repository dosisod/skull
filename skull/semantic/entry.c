#include <stdbool.h>
#include <string.h>

#include "skull/codegen/scope.h"
#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/range.h"

#include "skull/semantic/entry.h"

static bool _validate_ast_tree(AstNode *);
static bool validate_ast_node(AstNode *);
static bool validate_stmt_return(AstNode *);
static bool validate_stmt_func_decl(AstNode *);
static bool validate_stmt_type_alias(AstNode *);
bool assert_sane_child(AstNode *);
static void state_add_func(FunctionDeclaration *);

/*
Validate an entire AST tree starting at `node`.
*/
bool validate_ast_tree(AstNode *node) {
	const bool pass = _validate_ast_tree(node);
	reset_scope_head();

	free_scope(SKULL_STATE.scope);
	SKULL_STATE.scope = NULL;

	return pass;
}

/*
Validate an entire AST tree starting at `node`.
*/
static bool _validate_ast_tree(AstNode *node) {
	while (node) {
		if (node->child) {
			make_child_scope();
			const bool is_valid = _validate_ast_tree(node->child);
			restore_parent_scope();

			if (!is_valid) return false;
			make_adjacent_scope();
		}

		if (!validate_ast_node(node)) return false;
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

	return true;
}

static bool validate_stmt_return(AstNode *node) {
	return assert_sane_child(node->next);
}

static bool validate_stmt_func_decl(AstNode *node) {
	const bool is_external = node->func_proto->is_external;
	const bool is_export = node->func_proto->is_export;
	const Token *const func_name_token = node->func_proto->name_tok;

	if ((is_export || is_external) && !is_top_lvl_scope()) {
		FMT_ERROR(ERR_NO_NESTED, { .tok = func_name_token });
		return false;
	}

	char *func_name = token_mbs_str(func_name_token);

	if (is_export && strcmp(func_name, "main") == 0) {
		FMT_ERROR(ERR_MAIN_RESERVED, { .loc = &func_name_token->location });

		free(func_name);
		return false;
	}

	if (scope_find_name(SKULL_STATE.scope, func_name)) {
		FMT_ERROR(ERR_NO_REDEFINE_VAR_AS_FUNC, {
			.loc = &func_name_token->location,
			.real = func_name
		});

		return false;
	}

	FunctionDeclaration *found_func = find_func_by_name(func_name);

	if (found_func) {
		FMT_ERROR(ERR_NO_REDEFINE_FUNC, {
			.loc = &func_name_token->location,
			.real = func_name
		});

		return false;
	}

	Type return_type = NULL;
	char *return_type_name = node->func_proto->return_type_name;

	if (return_type_name)
		return_type = find_type(return_type_name);

	if (return_type_name && !return_type) {
		FMT_ERROR(ERR_TYPE_NOT_FOUND, { .str = return_type_name });

		free(func_name);
		return false;
	}

	FunctionDeclaration *func;
	func = Calloc(1, sizeof *func);
	func->name = func_name;
	func->location = func_name_token->location;
	func->is_external = is_external;
	func->is_export = is_export;
	func->return_type = return_type;
	func->param_names = node->func_proto->param_names;
	func->num_params = node->func_proto->num_params;

	node->func_proto->func = func;
	state_add_func(func);

	char **param_type_names = node->func_proto->param_type_names;
	if (!param_type_names) return true;

	func->param_types = Calloc(func->num_params, sizeof(Type));

	for RANGE(i, func->num_params) {
		func->param_types[i] = find_type(param_type_names[i]);

		if (!func->param_types[i]) {
			FMT_ERROR(ERR_TYPE_NOT_FOUND, { .str = param_type_names[i] });

			return false;
		}
	}

	return true;
}

static void state_add_func(FunctionDeclaration *func) {
	if (!SKULL_STATE.function_decls) {
		SKULL_STATE.function_decls = ht_create();
	}
	ht_add(SKULL_STATE.function_decls, func->name, func);
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
