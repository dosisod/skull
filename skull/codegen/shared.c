#include "skull/codegen/func.h"

#include "skull/codegen/shared.h"

SkullState SKULL_STATE;

/*
Free everything about a Skull compiler instance.
*/
void free_state(SkullState state) {
	free_ht(
		state.type_aliases,
		(void(*)(void *))free_ht_type_alias
	);
	free_ht(
		state.function_decls,
		(void(*)(void *))free_function_declaration
	);
	free_scope(state.scope);

	LLVMDisposeBuilder(state.builder);
	LLVMDisposeModule(state.module);
	LLVMContextDispose(state.ctx);
}
