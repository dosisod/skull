#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "skull/common/main.h"
#include "skull/eval/repl.h"

#include "skullc/llvm/ast.h"

int main(int argc, char *argv[]) {
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

	if (argc==1) {
		puts("expected filename, exiting");
		return 1;
	}
	HANDLE_MAIN;

	str_to_llvm_ir(repl_read_raw(f), builder, ctx);

	char *err = NULL;
	LLVMBool status = LLVMPrintModuleToFile(
		main_module,
		".tmp.ll",
		&err
	);

	if (err || status) {
		puts("error occurred!");
	}

	LLVMDisposeMessage(err);
	LLVMContextDispose(ctx);
	return 0;
}