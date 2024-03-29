#include <stdbool.h>
#include <string.h>

#include <llvm-c/Core.h>
#include <llvm-c/TargetMachine.h>

#include "skull/build_data.h"
#include "skull/codegen/abi.h"
#include "skull/codegen/llvm/debug.h"
#include "skull/codegen/llvm/types.h"
#include "skull/common/malloc.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/symbol.h"

#include "skull/codegen/llvm/shared.h"

SkullStateLLVM *setup_llvm_state(SemanticState *semantic_state) {
	SkullStateLLVM *state = Calloc(1, sizeof(SkullStateLLVM));

	const char *filename = BUILD_DATA.filename;

	LLVMContextRef ctx = LLVMContextCreate();
	LLVMModuleRef main_module = LLVMModuleCreateWithNameInContext(
		filename,
		ctx
	);
	state->ctx = ctx;

	LLVMTypeRef main_func_type = type_to_llvm_func_type(
		&TYPE_INT, NULL, 0, state
	);

	LLVMValueRef main_func = LLVMAddFunction(
		main_module,
		semantic_state->main_func->linkage_name,
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

	state->main_func = semantic_state->main_func;
	state->main_func->func->ref = main_func;
	state->main_func->func->type = main_func_type;
	state->current_func = state->main_func;
	state->semantic = semantic_state;

	if (BUILD_DATA.debug) setup_debug_info(filename, state);

	return state;
}

/*
Free everything about a Skull LLVM compiler instance.
*/
void free_llvm_state(SkullStateLLVM *state) {
	if (!state) return;

	LLVMDisposeBuilder(state->builder);
	LLVMDisposeModule(state->module);
	LLVMContextDispose(state->ctx);

	if (state->target_machine) LLVMDisposeTargetMachine(state->target_machine);

	free(state);
}
