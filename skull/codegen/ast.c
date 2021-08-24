#include <stdbool.h>

#include "skull/codegen/assign.h"
#include "skull/codegen/flow.h"
#include "skull/codegen/func.h"

#include "skull/codegen/ast.h"

static bool gen_expr_node(const AstNode *);
static Expr _gen_node(AstNode **, bool *);

/*
Run code generator for tree starting at `node`.
*/
bool gen_tree(AstNode *node) {
	bool err = false;
	const Expr expr = gen_node(node, &err);

	if (!expr.value && !err) {
		gen_stmt_implicit_main_return(&node->token->location);
	}

	return err;
}

/*
Return expr from an `AST_NODE_RETURN` if one was found.
*/
Expr gen_node(AstNode *node, bool *err) {
	while (node) {
		Expr parsed = _gen_node(&node, err);
		if (*err) break;

		if (parsed.value || parsed.type) return parsed;
		if (node) node = node->next;
	}

	return (Expr){0};
}

/*
Internal `gen_node` function.
*/
static Expr _gen_node(AstNode **node, bool *err) {
	switch ((*node)->type) {
		case AST_NODE_IF: *err |= gen_control_if(node); break;
		case AST_NODE_WHILE: *err |= gen_control_while(*node); break;
		case AST_NODE_RETURN: return gen_stmt_return(*node);
		case AST_NODE_UNREACHABLE: return gen_stmt_unreachable();
		case AST_NODE_FUNCTION_PROTO: *err |= gen_stmt_func_decl(*node); break;
		case AST_NODE_VAR_DEF:
			*err |= gen_stmt_var_def((*node)->var_def); break;
		case AST_NODE_VAR_ASSIGN:
			*err |= gen_stmt_var_assign((*node)->var_assign); break;
		case AST_NODE_EXPR: *err |= gen_expr_node(*node); break;
		case AST_NODE_NOOP: gen_stmt_noop(&(*node)->token->location); break;
		default: break;
	}

	return (Expr){0};
}

/*
Generate a (function) expression from `node`.

Return `true` if error occurred.
*/
static bool gen_expr_node(const AstNode *node) {
	Expr expr = gen_expr_func_call(node->expr->lhs.func_call);

	return !expr.value && !expr.type;
}
