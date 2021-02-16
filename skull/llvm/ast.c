#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/eval/scope.h"
#include "skull/llvm/aliases.h"
#include "skull/llvm/assign.h"
#include "skull/llvm/flow.h"
#include "skull/llvm/func.h"
#include "skull/llvm/shared.h"

#include "skull/llvm/ast.h"

/*
Generate LLVM from `str_`.
*/
void str_to_llvm(char *const str_) {
	char32_t *const str = mbstoc32s(str_);

	AstNode *const node = make_ast_tree(str);

	if (!node_to_llvm(node)) {
		LLVMBuildRet(SKULL_STATE.builder, LLVM_INT(0));
	}

	free_ast_tree(node);
	free(str);
}

/*
Internal LLVM parser.

Return true if there was a `AST_NODE_RETURN` node was parsed, else false.
*/
bool node_to_llvm(AstNode *node) {
	bool returned = false;

while (node) {
	const NodeType node_type = node->type;

	if (node_type == AST_NODE_IF) {
		gen_control_if(&node);
	}

	else if (node_type == AST_NODE_ELSE) {
		PANIC(ERR_ELSE_MISSING_IF, {0});
	}

	else if (node->child && !(
		node_type == AST_NODE_WHILE ||
		node_type == AST_NODE_FUNCTION ||
		node_type == AST_NODE_FUNCTION_PROTO
	)) {
		PANIC(ERR_UNEXPECTED_CODE_BLOCK, {0});
	}

	else if (node_type == AST_NODE_COMMENT) {}

	else if (returned) {
		PANIC(ERR_UNREACHALE_RETURN, {0});
	}

	else if (node_type == AST_NODE_RETURN) {
		gen_stmt_return(&node);
		returned = true;
	}

	else if (node_type == AST_NODE_UNREACHABLE) {
		LLVMBuildUnreachable(SKULL_STATE.builder);
		returned = true;
	}

	else if (node_type == AST_NODE_TYPE_ALIAS) {
		create_type_alias(&node);
	}

	else if (node_type == AST_NODE_VAR_DEF) {
		gen_stmt_var_def(&node);
	}

	else if (node_type == AST_NODE_WHILE) {
		gen_control_while(&node);
	}

	else if (node_type == AST_NODE_FUNCTION_PROTO) {
		gen_stmt_func_decl(node);
	}

	else if (node_type == AST_NODE_FUNCTION) {
		gen_expr_function_call(node, NULL);
	}

	else if (node_type == AST_NODE_VAR_ASSIGN) {
		gen_stmt_var_assign(&node);
	}

	else {
		PANIC(ERR_UNEXPECTED_TOKEN, { .tok = node->token });
	}

	node = node->next;
}

	return returned;
}
