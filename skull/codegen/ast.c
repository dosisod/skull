#include <stddef.h>
#include <string.h>

#include "skull/codegen/aliases.h"
#include "skull/codegen/assign.h"
#include "skull/codegen/flow.h"
#include "skull/codegen/func.h"
#include "skull/codegen/shared.h"
#include "skull/common/errors.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/compiler/scope.h"

#include "skull/codegen/ast.h"

/*
Generate code from `str_`.

Return `true` if errors occurred.
*/
bool codegen_str(char *const str_) {
	char32_t *const str = mbstoc32s(str_);

	AstNode *const node = make_ast_tree(str);
	if (!node) {
		free(str);
		return true;
	}

	if (!gen_node(node).value)
		LLVMBuildRet(SKULL_STATE.builder, LLVM_INT(0));

	free_ast_tree(node);
	free(str);

	return false;
}

static Expr _gen_node(AstNode **);

/*
Internal LLVM parser.

Return expr from an `AST_NODE_RETURN` if one was found.
*/
Expr gen_node(AstNode *node) {
	Expr returned = (Expr){0};

	while (node) {
		if (returned.value && !(
			node->type == AST_NODE_COMMENT ||
			node->type == AST_NODE_UNREACHABLE ||
			node->type == AST_NODE_NOOP
		)) {
			PANIC(ERR_UNREACHABLE_CODE, { .loc = &node->token->location });
		}

		Expr parsed = _gen_node(&node);
		if (!returned.value) returned = parsed;

		node = node->next;
	}

	return returned;
}

static void gen_expr_node(const AstNode *);

void assert_sane_child(AstNode *node) {
	if (!node) return;

	const NodeType node_type = node->type;

	if (node->child && !(
		node_type == AST_NODE_ELSE ||
		node_type == AST_NODE_FUNCTION_PROTO ||
		(node_type == AST_NODE_EXPR && node->expr->oper == EXPR_FUNC)
	)) {
		PANIC(ERR_UNEXPECTED_CODE_BLOCK, {
			.loc = &node->child->token->location
		});
	}
}

static Expr _gen_node(AstNode **node) {
	assert_sane_child(*node);

	switch ((*node)->type) {
		case AST_NODE_IF: gen_control_if(node); break;
		case AST_NODE_ELSE: {
			PANIC(ERR_ELSE_MISSING_IF, { .loc = &(*node)->token->location });
		}
		case AST_NODE_COMMENT:
		case AST_NODE_NOOP: break;
		case AST_NODE_RETURN: return gen_stmt_return(node);
		case AST_NODE_UNREACHABLE: return gen_stmt_unreachable();
		case AST_NODE_TYPE_ALIAS: create_type_alias(node); break;
		case AST_NODE_VAR_DEF: gen_stmt_var_def(node); break;
		case AST_NODE_WHILE: gen_control_while(node); break;
		case AST_NODE_FUNCTION_PROTO: gen_stmt_func_decl(*node); break;
		case AST_NODE_VAR_ASSIGN: gen_stmt_var_assign(node); break;
		case AST_NODE_EXPR: gen_expr_node(*node); break;
		default: {
			if ((*node)->token) {
				PANIC(ERR_UNEXPECTED_TOKEN, { .tok = (*node)->token });
			}
		}
	}

	return (Expr){0};
}

static void gen_expr_node(const AstNode *node) {
	if (node->expr->oper != EXPR_FUNC) {
		PANIC(ERR_NO_DANGLING_EXPR, { .loc = &node->token->location });
	}

	gen_expr_function_call(node->expr, NULL);
}
