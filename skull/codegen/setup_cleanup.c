#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "skull/codegen/ast.h"
#include "skull/codegen/func.h"
#include "skull/codegen/shared.h"
#include "skull/common/malloc.h"

#include "skull/codegen/setup_cleanup.h"

/*
Write LLVM code to `filename`, return whether error occured.

Function takes control of `filename`.
*/
int write_file(char *filename) {
	char *err = NULL;
	LLVMBool status = LLVMPrintModuleToFile(
		SKULL_STATE.module,
		filename,
		&err
	);

	free_state(SKULL_STATE);
	free(filename);

	if (err || status) {
		fprintf(stderr, "skull: error occurred: %s\n", err);
		LLVMDisposeMessage(err);
		return 1;
	}

	LLVMDisposeMessage(err);
	return 0;
}

/*
Create a module named `filename` from `file_contents`.
*/
void generate_llvm(
	const char *filename,
	char *file_contents
) {
	char *main_func_name = create_llvm_main_func(filename);

	LLVMModuleRef main_module = LLVMModuleCreateWithName(filename);

	LLVMContextRef ctx = LLVMContextCreate();

	LLVMTypeRef main_func_type = LLVMFunctionType(
		LLVMInt64TypeInContext(ctx),
		NULL,
		0,
		false
	);

	LLVMValueRef main_func = LLVMAddFunction(
		main_module,
		main_func_name,
		main_func_type
	);
	free(main_func_name);

	LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(
		ctx,
		main_func,
		"entry"
	);

	LLVMBuilderRef builder = LLVMCreateBuilderInContext(ctx);

	LLVMPositionBuilderAtEnd(
		builder,
		entry
	);

	SKULL_STATE = (SkullState){
		.builder = builder,
		.ctx = ctx,
		.module = main_module,
		.filename = filename
	};

	SKULL_STATE.main_func = (FunctionDeclaration){
		.name = main_func_name,
		.function = main_func,
		.type = main_func_type,
		.return_type = TYPE_INT
	};
	SKULL_STATE.current_func = &SKULL_STATE.main_func;

	codegen_str(file_contents);
}

/*
Convert/mangle `filename` into suitable name for "main" method for module.
*/
char *create_llvm_main_func(const char *filename) {
	char *slash_pos = strrchr(filename, '/');

	if (slash_pos) {
		filename = slash_pos + 1;
	}

	const size_t len = strlen(filename) - 1;

	char *ret = Malloc(len);
	ret[0] = '.';
	strncpy(ret + 1, filename, len - 1);
	ret[len - 1] = '\0';

	return ret;
}
