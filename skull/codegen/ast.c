#include <stddef.h>
#include <string.h>

#include "skull/codegen/aliases.h"
#include "skull/codegen/assign.h"
#include "skull/codegen/flow.h"
#include "skull/codegen/func.h"
#include "skull/codegen/shared.h"
#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/compiler/scope.h"

#include "skull/codegen/ast.h"

/*
Generate code from `str_`.

Return `true` if errors occurred.
*/
bool codegen_str(char *const str_) {
	char32_t *const str = mbstoc32s(str_);
	if (!str) {
		return true;
	}

	AstNode *const node = make_ast_tree(str);
	if (!node) {
		free(str);
		return true;
	}

	bool err = false;
	const Expr expr = gen_node(node, &err);

	if (!expr.value && !err)
		LLVMBuildRet(SKULL_STATE.builder, LLVM_INT(0));

	free_ast_tree(node);
	free(str);

	return err;
}

static Expr _gen_node(AstNode **, bool *);

/*
Internal LLVM parser.

Return expr from an `AST_NODE_RETURN` if one was found.
*/
Expr gen_node(AstNode *node, bool *err) {
	Expr returned = (Expr){0};

	while (node) {
		if (returned.value && !(
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
		node = node->next;
	}

	return returned;
}

static bool gen_expr_node(const AstNode *);

/*
Verify that `node` doens't contain child node if it shouldn't.

Return `true` if node is "sane".
*/
bool assert_sane_child(AstNode *node) {
	if (!node) return true;

	const NodeType node_type = node->type;

	if (node->child && !(
		node_type == AST_NODE_ELSE ||
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
		case AST_NODE_ELSE: {
			FMT_ERROR(ERR_ELSE_MISSING_IF, {
				.loc = &(*node)->token->location
			});

			*err = true;
			break;
		}
		case AST_NODE_WHILE: *err |= gen_control_while(node); break;
		case AST_NODE_RETURN: return gen_stmt_return(node, err);
		case AST_NODE_UNREACHABLE: return gen_stmt_unreachable();
		case AST_NODE_TYPE_ALIAS: *err |= create_type_alias(node); break;
		case AST_NODE_FUNCTION_PROTO: *err |= gen_stmt_func_decl(*node); break;
		case AST_NODE_VAR_DEF: *err |= gen_stmt_var_def(node); break;
		case AST_NODE_VAR_ASSIGN: *err |= gen_stmt_var_assign(node); break;
		case AST_NODE_EXPR: *err |= gen_expr_node(*node); break;
		case AST_NODE_COMMENT:
		case AST_NODE_NOOP: break;
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
	if (node->expr->oper != EXPR_FUNC) {
		FMT_ERROR(ERR_NO_DANGLING_EXPR, { .loc = &node->token->location });

		return true;
	}

	bool err = false;
	gen_expr_function_call(node->expr, NULL, &err);
	return err;
}
