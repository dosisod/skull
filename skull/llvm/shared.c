#include "skull/llvm/func.h"

#include "skull/llvm/shared.h"

void free_state(SkullState state) {
	free_ht(state.type_aliases, NULL);
	free_ht(
		state.function_decls,
		(void(*)(void *))free_function_declaration
	);
	free_scope(state.scope);

	LLVMDisposeBuilder(state.builder);
	LLVMDisposeModule(state.module);
	LLVMContextDispose(state.ctx);
}
