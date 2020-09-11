#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "skull/common/io.h"
#include "skull/common/main.h"
#include "skull/llvm/ast.h"

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

	if (argc == 1) {
		puts("expected filename, exiting");
		return 1;
	}
	HANDLE_MAIN;

	str_to_llvm_ir(
		read_file(f, false),
		main_func,
		builder,
		ctx
	);

	size_t len = strlen(argv[1]);
	char *ll_filename = malloc(len + 5); // NOLINT

	char *slash_pos = strrchr(argv[1], '/');
	if (!slash_pos) {
		ll_filename[0] = '.';
		memcpy(ll_filename + 1, argv[1], len);
	}
	else {
		long offset = slash_pos - argv[1];

		memcpy(ll_filename, argv[1], len);
		ll_filename[offset + 1] = '.';
		memcpy(ll_filename + offset + 2, slash_pos + 1, len - (size_t)offset);
	}
	memcpy(ll_filename + len + 1, ".ll\0", 4);

	char *err = NULL;
	LLVMBool status = LLVMPrintModuleToFile(
		main_module,
		ll_filename,
		&err
	);
	free(ll_filename);

	if (err || status) {
		printf("error occurred: %s\n", err);
		LLVMDisposeMessage(err);
		return 1;
	}

	LLVMDisposeMessage(err);
	LLVMContextDispose(ctx);
	return 0;
}