#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "skull/codegen/llvm/assign.h"
#include "skull/codegen/llvm/flow.h"
#include "skull/codegen/llvm/func.h"

#include "skull/codegen/llvm/core.h"

static Expr gen_node(const AstNode **, SkullStateLLVM *);

/*
Run code generator for module starting at `node`.
*/
void gen_module(const AstNode *node, SkullStateLLVM *state) {
	const Expr expr = gen_tree(node, state);

	if (!expr.value) {
		gen_stmt_implicit_main_return(&node->token->location, state);
	}
}

/*
Generate tree starting at `node`, returning an expr if one is returned.
*/
Expr gen_tree(const AstNode *node, SkullStateLLVM *state) {
	while (node) {
		Expr parsed = gen_node(&node, state);

		if (parsed.value || parsed.type) return parsed;
		if (node) node = node->next;
	}

	return (Expr){0};
}

static Expr gen_node(const AstNode **node, SkullStateLLVM *state) {
	switch ((*node)->type) {
		case AST_NODE_IF: gen_control_if(node, state); break;
		case AST_NODE_WHILE: gen_control_while(*node, state); break;
		case AST_NODE_RETURN: return gen_stmt_return(*node, state);
		case AST_NODE_UNREACHABLE: return gen_stmt_unreachable(state);
		case AST_NODE_FUNCTION_PROTO: gen_stmt_func_decl(*node, state); break;
		case AST_NODE_VAR_DEF:
			gen_stmt_var_def((*node)->var_def, state); break;
		case AST_NODE_VAR_ASSIGN:
			gen_stmt_var_assign((*node)->var_assign, state); break;
		case AST_NODE_EXPR:
			gen_expr_func_call((*node)->expr->lhs.func_call, state); break;
		case AST_NODE_NOOP:
			gen_stmt_noop(&(*node)->token->location, state); break;
		case AST_NODE_BREAK: return gen_stmt_break(state);
		case AST_NODE_CONTINUE: return gen_stmt_continue(state);
		case AST_NODE_NAMESPACE: gen_control_namespace(*node, state); break;
		case AST_NODE_IMPORT:
		case AST_NODE_COMMENT:
			break;
		default: assert(false);
	}

	return (Expr){0};
}
