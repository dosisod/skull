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

#include "skull/llvm/ast.h"

LLVMModuleRef MODULE;
LLVMValueRef CURRENT_FUNC;
LLVMValueRef MAIN_FUNC;
LLVMBuilderRef BUILDER;
Scope *SCOPE;

/*
Convert skull code from `str_` into LLVM IR (using `func` and `module`).
*/
void str_to_llvm_ir(char *const str_, LLVMValueRef func, LLVMModuleRef module, LLVMBuilderRef builder) {
	char32_t *const str = mbstoc32s(str_);

	AstNode *const node = make_ast_tree(str);

	SCOPE = make_scope();
	CURRENT_FUNC = func;
	MAIN_FUNC = func;
	MODULE = module;
	BUILDER = builder;

	if (!node_to_llvm_ir(node)) {
		LLVMBuildRet(BUILDER, LLVM_INT(0));
	}

	free_ast_tree(node);
	free(str);

	free_function_declarations(FUNCTION_DECLARATIONS);
	free_scope(SCOPE);
}

/*
Internal LLVM IR parser.

Return true if there was a `AST_NODE_RETURN` node was parsed, else false.
*/
bool node_to_llvm_ir(AstNode *node) {
	bool returned = false;

while (node) {
	const NodeType node_type = node->type;

	if (node_type == AST_NODE_IF) {
		llvm_make_if(&node);
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
		llvm_make_return(&node);
		returned = true;
	}

	else if (node_type == AST_NODE_TYPE_ALIAS) {
		llvm_make_type_alias(&node);
	}

	else if (node_type == AST_NODE_VAR_DEF) {
		llvm_make_var_def(&node);
	}

	else if (node_type == AST_NODE_WHILE) {
		llvm_make_while(&node);
	}

	else if (node_type == AST_NODE_FUNCTION_PROTO) {
		declare_function(node);
	}

	else if (node_type == AST_NODE_FUNCTION) {
		llvm_make_function_call(node);
	}

	else if (node_type == AST_NODE_VAR_ASSIGN) {
		llvm_make_var_assign(&node);
	}

	else {
		PANIC(ERR_UNEXPECTED_TOKEN, { .tok = node->token });
	}

	node = node->next;
}

	return returned;
}