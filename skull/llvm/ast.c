#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/eval/scope.h"
#include "skull/llvm/aliases.h"
#include "skull/llvm/flow.h"
#include "skull/llvm/func.h"
#include "skull/llvm/panic.h"
#include "skull/parse/classify.h"

#include "skull/llvm/ast.h"

LLVMModuleRef module;
LLVMValueRef func;
Scope *scope;
LLVMBuilderRef builder;

/*
Convert skull code from `str_` into LLVM IR (using `func_` and `module_`).
*/
void str_to_llvm_ir(char *const str_, LLVMValueRef func_, LLVMModuleRef module_, LLVMBuilderRef _builder) {
	char32_t *const str = mbstoc32s(str_);
	DIE_IF_MALLOC_FAILS(str);

	char32_t *error = NULL;
	AstNode *const node = make_ast_tree(str, &error);

	if (!node) {
		PANIC(error);
	}

	scope = make_scope();
	func = func_;
	module = module_;
	builder = _builder;

	node_to_llvm_ir(node);
	free_ast_tree(node);
	free(str);

	ExternalFunction *f = external_functions;
	while (f && f->next) {
		free(f->name);

		f = f->next;
	}
}

/*
Internal LLVM IR parser.
*/
void node_to_llvm_ir(AstNode *node) {
	while (node) {
		if (node->node_type == AST_NODE_COMMENT) {}

		else if (node->node_type == AST_NODE_RETURN) {
			llvm_make_return(node);
		}

		else if (node->node_type == AST_NODE_VAR_DEF) {
			llvm_make_var_def(&node);
		}

		else if (node->node_type == AST_NODE_IF) {
			llvm_make_if(node);
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
			PANIC(FMT_ERROR(ERR_UNEXPECTED_TOKEN, { .tok = node->token }));
		}

		node = node->next;
	}
}