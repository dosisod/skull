#include <stdbool.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/semantic/assign.h"
#include "skull/semantic/expr.h"
#include "skull/semantic/flow.h"
#include "skull/semantic/func.h"
#include "skull/semantic/import.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/symbol.h"
#include "skull/semantic/unused.h"

#include "skull/semantic/entry.h"


static bool validate_ast_node(SemanticState *, const AstNode *);
static bool validate_ast_sub_tree(SemanticState *, const AstNode *);

/*
Validate an entire AST tree starting at `node` (the root).
*/
bool validate_ast_tree(SemanticState *state, const AstNode *node) {
	const bool pass = validate_ast_sub_tree(state, node);
	reset_scope_head(state);

	return pass && check_unused_symbols(node);
}

/*
Validate an AST tree starting at `node`.
*/
static bool validate_ast_sub_tree(SemanticState *state, const AstNode *node) {
	while (node) {
		if (!validate_ast_node(state, node)) return false;

		node = node->next;
	}

	reset_scope_head(state);
	return true;
}

bool setup_and_validate_ast_sub_tree(
	SemanticState *state,
	const AstNode *node
) {
	make_child_scope(state);
	const bool is_valid = validate_ast_sub_tree(state, node);
	restore_parent_scope(state);

	if (!is_valid) return false;
	make_adjacent_scope(state);

	return true;
}

/*
Validate a single AST `node`.
*/
static bool validate_ast_node(SemanticState *state, const AstNode *node) {
	if (!assert_sane_child(node)) return false;

	switch (node->type) {
		case AST_NODE_RETURN: return validate_stmt_return(state, node);
		case AST_NODE_UNREACHABLE: return validate_stmt_unreachable(node);
		case AST_NODE_FUNCTION_PROTO:
			return validate_stmt_func_decl(state, node);
		case AST_NODE_VAR_DEF: return validate_stmt_var_def(state, node);
		case AST_NODE_VAR_ASSIGN: return validate_stmt_var_assign(state, node);
		case AST_NODE_EXPR: return validate_expr_func(state, node);
		case AST_NODE_IF: return validate_control_if(state, node);
		case AST_NODE_ELSE: return validate_control_else(state, node);
		case AST_NODE_ELIF: return validate_control_elif(state, node);
		case AST_NODE_WHILE: return validate_control_while(state, node);
		case AST_NODE_BREAK: return validate_control_break(state, node);
		case AST_NODE_CONTINUE: return validate_control_continue(state, node);
		case AST_NODE_IMPORT: return validate_stmt_import(node);
		default: return true;
	}
}
