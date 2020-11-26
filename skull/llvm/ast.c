#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/panic.h"
#include "skull/common/str.h"
#include "skull/eval/scope.h"
#include "skull/llvm/aliases.h"
#include "skull/llvm/assign.h"
#include "skull/llvm/flow.h"
#include "skull/llvm/func.h"
#include "skull/parse/classify.h"

#include "skull/llvm/ast.h"

LLVMModuleRef MODULE;
LLVMValueRef FUNC;
Scope *SCOPE;
LLVMBuilderRef BUILDER;

/*
Convert skull code from `str_` into LLVM IR (using `func` and `module`).
*/
void str_to_llvm_ir(char *const str_, LLVMValueRef func, LLVMModuleRef module, LLVMBuilderRef builder) {
	char32_t *const str = mbstoc32s(str_);
	DIE_IF_MALLOC_FAILS(str);

	AstNode *const node = make_ast_tree(str);

	SCOPE = make_scope();
	FUNC = func;
	MODULE = module;
	BUILDER = builder;

	if (!node_to_llvm_ir(node)) {
		LLVMBuildRet(BUILDER, LLVM_INT(0));
	}

	free_ast_tree(node);
	free(str);

	ExternalFunction *f = EXTERNAL_FUNCTIONS;
	while (f) {
		free(f->name);

		ExternalFunction *copy = f;
		f = f->next;
		free(copy);
	}

	free(SCOPE);
}

/*
Internal LLVM IR parser.

Return true if there was a `AST_NODE_RETURN` node was parsed, else false.
*/
bool node_to_llvm_ir(AstNode *node) {
	while (node) {
		if (node->node_type == AST_NODE_COMMENT) {}

		else if (node->node_type == AST_NODE_RETURN) {
			llvm_make_return(node);
			return true;
		}

		else if (node->node_type == AST_NODE_VAR_DEF) {
			llvm_make_var_def(&node);
		}

		else if (node->node_type == AST_NODE_IF) {
			llvm_make_if(&node);
		}

		else if (node->node_type == AST_NODE_WHILE) {
			llvm_make_while(node);
		}

		else if (node->node_type == AST_NODE_FUNCTION_PROTO) {
			declare_external_function(node);
		}

		else if (node->node_type == AST_NODE_FUNCTION) {
			llvm_make_function(node);
		}

		else if (node->node_type == AST_NODE_VAR_ASSIGN) {
			llvm_make_assign(&node);
		}

		else {
			PANIC(ERR_UNEXPECTED_TOKEN, { .tok = node->token });
		}

		node = node->next;
	}
	return false;
}