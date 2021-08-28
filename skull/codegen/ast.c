#include "skull/codegen/assign.h"
#include "skull/codegen/flow.h"
#include "skull/codegen/func.h"

#include "skull/codegen/ast.h"

static Expr _gen_tree(AstNode **);

/*
Run code generator for module starting at `node`.
*/
void gen_module(AstNode *node) {
	const Expr expr = gen_tree(node);

	if (!expr.value) {
		gen_stmt_implicit_main_return(&node->token->location);
	}
}

/*
Generate tree starting at `node`, returning an expr if one is returned.
*/
Expr gen_tree(AstNode *node) {
	while (node) {
		Expr parsed = _gen_tree(&node);

		if (parsed.value || parsed.type) return parsed;
		if (node) node = node->next;
	}

	return (Expr){0};
}

/*
Internal `gen_tree` function.
*/
static Expr _gen_tree(AstNode **node) {
	switch ((*node)->type) {
		case AST_NODE_IF: gen_control_if(node); break;
		case AST_NODE_WHILE: gen_control_while(*node); break;
		case AST_NODE_RETURN: return gen_stmt_return(*node);
		case AST_NODE_UNREACHABLE: return gen_stmt_unreachable();
		case AST_NODE_FUNCTION_PROTO: gen_stmt_func_decl(*node); break;
		case AST_NODE_VAR_DEF: gen_stmt_var_def((*node)->var_def); break;
		case AST_NODE_VAR_ASSIGN:
			gen_stmt_var_assign((*node)->var_assign); break;
		case AST_NODE_EXPR:
			gen_expr_func_call((*node)->expr->lhs.func_call); break;
		case AST_NODE_NOOP: gen_stmt_noop(&(*node)->token->location); break;
		default: break;
	}

	return (Expr){0};
}
