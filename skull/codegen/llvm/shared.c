#include <stdbool.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/build_data.h"
#include "skull/codegen/abi.h"
#include "skull/codegen/llvm/debug.h"
#include "skull/codegen/llvm/types.h"
#include "skull/common/malloc.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"

#include "skull/codegen/llvm/shared.h"

SkullStateLLVM SKULL_STATE_LLVM;

void setup_llvm_state(void) {
	SkullStateLLVM *state = &SKULL_STATE_LLVM;

	const char *filename = BUILD_DATA.filename;

	LLVMContextRef ctx = LLVMContextCreate();
	LLVMModuleRef main_module = LLVMModuleCreateWithNameInContext(
		filename,
		ctx
	);
	state->ctx = ctx;

	LLVMTypeRef main_func_type = type_to_llvm_func_type(&TYPE_INT, NULL, 0);

	LLVMValueRef main_func = LLVMAddFunction(
		main_module,
		SEMANTIC_STATE.main_func->name,
		main_func_type
	);

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

	state->main_func = SEMANTIC_STATE.main_func;
	state->main_func->ref = main_func;
	state->main_func->type = main_func_type;

	state->current_func = state->main_func;

	if (BUILD_DATA.debug) {
		setup_debug_info(filename, SKULL_STATE_LLVM.module);
	}
}

/*
Free everything about a Skull LLVM compiler instance.
*/
void free_llvm_state(void) {
	SkullStateLLVM *state = &SKULL_STATE_LLVM;

	LLVMDisposeBuilder(state->builder);
	LLVMDisposeModule(state->module);
	LLVMContextDispose(state->ctx);

	*state = (SkullStateLLVM){0};
}
