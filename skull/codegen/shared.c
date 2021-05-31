#include <stdlib.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/codegen/func.h"
#include "skull/codegen/llvm/types.h"
#include "skull/common/malloc.h"

#include "skull/codegen/shared.h"

SkullState SKULL_STATE;

static char *create_main_func_name(const char *);

void setup_state(SkullState *state, const char *filename) {
	char *main_func_name = create_main_func_name(filename);

	LLVMModuleRef main_module = LLVMModuleCreateWithName(filename);
	LLVMContextRef ctx = LLVMContextCreate();
	state->ctx = ctx;

	LLVMTypeRef main_func_type = gen_llvm_func_type(TYPE_INT, NULL, 0);

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

	const bool c_backend = SKULL_STATE.c_backend;

	*state = (SkullState){
		.builder = builder,
		.ctx = ctx,
		.module = main_module,
		.filename = filename
	};

	SKULL_STATE.c_backend = c_backend;

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
static char *create_main_func_name(const char *filename) {
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
Free everything about a Skull compiler instance.
*/
void free_state(SkullState *state) {
	free_ht(
		state->type_aliases,
		(void(*)(void *))free_ht_type_alias
	);
	free_ht(
		state->function_decls,
		(void(*)(void *))free_function_declaration
	);
	free_scope(state->scope);

	LLVMDisposeBuilder(state->builder);
	LLVMDisposeModule(state->module);
	LLVMContextDispose(state->ctx);

	free(state->main_func);
}
