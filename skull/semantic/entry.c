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

#include "skull/semantic/entry.h"


static bool _validate_ast_tree(const AstNode *);
static bool validate_ast_node(const AstNode *);
static bool validate_stmt_type_alias(const AstNode *);

/*
Validate an entire AST tree starting at `node`.
*/
bool validate_ast_tree(const AstNode *node) {
	const bool pass = _validate_ast_tree(node);
	reset_scope_head();

	return pass;
}

/*
Validate an entire AST tree starting at `node`.
*/
static bool _validate_ast_tree(const AstNode *node) {
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
static bool validate_ast_node(const AstNode *node) {
	switch (node->type) {
		case AST_NODE_RETURN: return validate_stmt_return(node);
		case AST_NODE_FUNCTION_PROTO: return validate_stmt_func_decl(node);
		case AST_NODE_TYPE_ALIAS: return validate_stmt_type_alias(node);
		case AST_NODE_VAR_DEF: return validate_stmt_var_def(node);
		case AST_NODE_VAR_ASSIGN: return validate_stmt_var_assign(node);
		case AST_NODE_EXPR: return validate_expr_func(node);
		case AST_NODE_IF: return validate_control_if(node);
		case AST_NODE_ELSE: return validate_control_else(node);
		case AST_NODE_ELIF: return validate_control_elif(node);
		case AST_NODE_WHILE: return validate_control_while(node);
		default: return true;
	}
}

static bool validate_stmt_type_alias(const AstNode *node) {
	const Token *const token = node->token;

	char *type_name = token_to_mbs_str(token->next->next);
	char *alias = token_to_mbs_str(token);

	Symbol *symbol;
	symbol = Calloc(1, sizeof *symbol);
	*symbol = (Symbol){
		.name = alias,
		.location = &token->location,
		.expr_type = find_type(type_name),
		.type = SYMBOL_ALIAS,
	};

	if (scope_add_symbol(symbol)) {
		free(type_name);
		return true;
	}

	free(type_name);
	free(symbol);
	return false;
}

