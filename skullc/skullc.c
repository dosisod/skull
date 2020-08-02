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

	LLVMValueRef num = LLVMConstInt(
		LLVMInt64TypeInContext(ctx),
		1,
		true
	);

	LLVMValueRef ret = LLVMBuildRet(builder, num);

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