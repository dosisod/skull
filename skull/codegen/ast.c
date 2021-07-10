#include <stdbool.h>

#include "skull/codegen/assign.h"
#include "skull/codegen/flow.h"
#include "skull/codegen/func.h"
#include "skull/common/errors.h"

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
	Expr returned = (Expr){0};

	while (node) {
		if ((returned.value || returned.type) && !(
			node->type == AST_NODE_COMMENT ||
			node->type == AST_NODE_UNREACHABLE ||
			node->type == AST_NODE_NOOP
		)) {
			FMT_ERROR(ERR_UNREACHABLE_CODE, { .loc = &node->token->location });
			*err = true;
			break;
		}

		Expr parsed = _gen_node(&node, err);
		if (*err) break;

		if (!returned.value) returned = parsed;
		if (node) node = node->next;
	}

	return returned;
}

/*
Verify that `node` doens't contain child node if it shouldn't.

Return `true` if node is "sane".
*/
bool assert_sane_child(AstNode *node) {
	if (!node) return true;

	const NodeType node_type = node->type;

	if (node->child && !(
		node_type == AST_NODE_IF ||
		node_type == AST_NODE_ELIF ||
		node_type == AST_NODE_ELSE ||
		node_type == AST_NODE_WHILE ||
		node_type == AST_NODE_FUNCTION_PROTO ||
		(node_type == AST_NODE_EXPR && node->expr->oper == EXPR_FUNC)
	)) {
		FMT_ERROR(ERR_UNEXPECTED_CODE_BLOCK, {
			.loc = &node->child->token->location
		});

		return false;
	}

	return true;
}

/*
Internal `gen_node` function.
*/
static Expr _gen_node(AstNode **node, bool *err) {
	if (!assert_sane_child(*node)) {
		*err = true;
		return (Expr){0};
	}

	switch ((*node)->type) {
		case AST_NODE_IF: *err |= gen_control_if(node); break;
		case AST_NODE_WHILE: *err |= gen_control_while(*node); break;
		case AST_NODE_RETURN: return gen_stmt_return(*node, err);
		case AST_NODE_UNREACHABLE: return gen_stmt_unreachable();
		case AST_NODE_FUNCTION_PROTO: *err |= gen_stmt_func_decl(*node); break;
		case AST_NODE_VAR_DEF: *err |= gen_stmt_var_def(*node); break;
		case AST_NODE_VAR_ASSIGN: *err |= gen_stmt_var_assign(*node); break;
		case AST_NODE_EXPR: *err |= gen_expr_node(*node); break;
		case AST_NODE_COMMENT:
		case AST_NODE_NOOP:
		case AST_NODE_TYPE_ALIAS: break;
		default: {
			if ((*node)->token) {
				FMT_ERROR(ERR_UNEXPECTED_TOKEN, { .tok = (*node)->token });
				*err = true;
			}
		}
	}

	return (Expr){0};
}

/*
Generate a (function) expression from `node`.

Return `true` if error occurred.
*/
static bool gen_expr_node(const AstNode *node) {
	bool err = false;
	gen_expr_function_call(node->expr, NULL, &err);
	return err;
}
