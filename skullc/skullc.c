#include <stdbool.h>
#include <stdio.h>

#include <llvm-c/Core.h>

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
		""
	);

	LLVMBuilderRef builder = LLVMCreateBuilderInContext(ctx);

	LLVMPositionBuilderAtEnd(
		builder,
		block
	);

	LLVMValueRef alloc = LLVMBuildAlloca(
		builder,
		LLVMInt64TypeInContext(ctx),
		"x"
	);

	LLVMBuildStore(
		builder,
		LLVMConstInt(
			LLVMInt64TypeInContext(ctx),
			1,
			true
		),
		alloc
	);

	LLVMValueRef load = LLVMBuildLoad(
		builder,
		alloc,
		"1"
	);

	LLVMValueRef added = LLVMBuildAdd(
		builder,
		load,
		LLVMConstInt(
			LLVMInt64TypeInContext(ctx),
			1,
			true
		),
		"2"
	);

	LLVMBuildRet(builder, added);

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