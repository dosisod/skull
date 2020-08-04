#include <stdbool.h>
#include <stdio.h>

#include <llvm-c/Core.h>

#include "skull/eval/eval_integer.h"
#include "skull/parse/ast/node.h"

int main(void) {
	LLVMContextRef ctx = LLVMContextCreate();
	LLVMModuleRef main_module = LLVMModuleCreateWithNameInContext("main_module", ctx);

	LLVMTypeRef main_func_type = LLVMFunctionType(
		LLVMInt64Type(),
		NULL,
		0,
		false
	);

	LLVMValueRef main_func = LLVMAddFunction(
		main_module,
		"main",
		main_func_type
	);

	LLVMBasicBlockRef block = LLVMAppendBasicBlockInContext(
		ctx,
		main_func,
		"entry"
	);

	LLVMBuilderRef builder = LLVMCreateBuilderInContext(ctx);

	LLVMPositionBuilderAtEnd(
		builder,
		block
	);

	ast_node_t *node=make_ast_tree(U"return 1");

	if (node->node_type==AST_NODE_RETURN) {
		const char32_t *error = NULL;
		int64_t *num = eval_integer(node->token->next, &error);

		LLVMBuildRet(
			builder,
			LLVMConstInt(
				LLVMInt64TypeInContext(ctx),
				*num,
				true
			)
		);
	}

	char *err = NULL;
	LLVMBool status = LLVMPrintModuleToFile(
		main_module,
		".tmp.ll",
		&err
	);

	if (err!=NULL || status) {
		puts("error occurred!");
	}

	LLVMDisposeMessage(err);
	LLVMContextDispose(ctx);
	return 0;
}