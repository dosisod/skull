#include <stdbool.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/build_data.h"
#include "skull/codegen/llvm/types.h"
#include "skull/common/malloc.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"

#include "skull/codegen/llvm/shared.h"

SkullStateLLVM SKULL_STATE_LLVM;

void setup_llvm_state(void) {
	SkullStateLLVM *state = &SKULL_STATE_LLVM;

	const char *filename = BUILD_DATA.filename;
	char *main_func_name = create_main_func_name(filename);

	LLVMContextRef ctx = LLVMContextCreate();
	LLVMModuleRef main_module = LLVMModuleCreateWithNameInContext(
		filename,
		ctx
	);
	state->ctx = ctx;

	LLVMTypeRef main_func_type = type_to_llvm_func_type(TYPE_INT, NULL, 0);

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

	LLVMPositionBuilderAtEnd(builder, entry);

	*state = (SkullStateLLVM){
		.builder = builder,
		.ctx = ctx,
		.module = main_module
	};

	state->main_func = Malloc(sizeof *state->main_func);
	*state->main_func = (FunctionDeclaration){
		.name = main_func_name,
		.ref = main_func,
		.type = main_func_type,
		.return_type = TYPE_INT
	};
	state->current_func = state->main_func;
}

/*
Convert/mangle `filename` into suitable name for "main" method for module.
*/
char *create_main_func_name(const char *filename) {
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

/*
Free everything about a Skull LLVM compiler instance.
*/
void free_llvm_state(void) {
	SkullStateLLVM *state = &SKULL_STATE_LLVM;

	LLVMDisposeBuilder(state->builder);
	LLVMDisposeModule(state->module);
	LLVMContextDispose(state->ctx);

	free(state->main_func);
}
