#include <stdbool.h>
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
*/
void codegen_str(char *const str_) {
	char32_t *const str = mbstoc32s(str_);

	AstNode *const node = make_ast_tree(str);

	if (!gen_node(node))
		LLVMBuildRet(SKULL_STATE.builder, LLVM_INT(0));

	free_ast_tree(node);
	free(str);
}

bool _gen_node(AstNode **);

/*
Internal LLVM parser.

Return true if there was a `AST_NODE_RETURN` node was parsed, else false.
*/
bool gen_node(AstNode *node) {
	bool already_returned = false;

	while (node) {
		if (already_returned) {
			PANIC(ERR_UNREACHALE_RETURN, {0});
		}

		already_returned = _gen_node(&node);
		node = node->next;
	}

	return already_returned;
}

bool _gen_node(AstNode **node) {
	const NodeType node_type = (*node)->type;

	if (node_type == AST_NODE_IF)
		gen_control_if(node);

	else if (node_type == AST_NODE_ELSE) {
		PANIC(ERR_ELSE_MISSING_IF, {0});
	}

	else if ((*node)->child && !(
		node_type == AST_NODE_WHILE ||
		(node_type == AST_NODE_EXPR && (*node)->attr.expr->oper == EXPR_FUNC) ||
		node_type == AST_NODE_FUNCTION_PROTO
	)) {
		PANIC(ERR_UNEXPECTED_CODE_BLOCK, {0});
	}

	else if (node_type == AST_NODE_COMMENT) {}

	else if (node_type == AST_NODE_RETURN) {
		gen_stmt_return(node);
		return true;
	}

	else if (node_type == AST_NODE_UNREACHABLE) {
		LLVMBuildUnreachable(SKULL_STATE.builder);
		return true;
	}

	else if (node_type == AST_NODE_TYPE_ALIAS)
		create_type_alias(node);

	else if (node_type == AST_NODE_VAR_DEF)
		gen_stmt_var_def(node);

	else if (node_type == AST_NODE_WHILE)
		gen_control_while(node);

	else if (node_type == AST_NODE_FUNCTION_PROTO)
		gen_stmt_func_decl(*node);

	else if (node_type == AST_NODE_EXPR && (*node)->attr.expr->oper == EXPR_FUNC)
		gen_expr_function_call((*node)->attr.expr, NULL);

	else if (node_type == AST_NODE_VAR_ASSIGN)
		gen_stmt_var_assign(node);

	else if ((*node)->token) {
		PANIC(ERR_UNEXPECTED_TOKEN, { .tok = (*node)->token });
	}

	return false;
}
