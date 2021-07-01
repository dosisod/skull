#include <stdbool.h>
#include <string.h>

#include <llvm-c/Core.h>

#include "skull/build_data.h"
#include "skull/codegen/llvm/types.h"
#include "skull/codegen/scope.h"
#include "skull/common/malloc.h"
#include "skull/semantic/func.h"

#include "skull/codegen/shared.h"

SkullState SKULL_STATE;

/*
Free everything about a Skull compiler instance.
*/
void free_state(void) {
	SkullState *state = &SKULL_STATE;

	free_ht(
		state->type_aliases,
		(void(*)(void *))free_ht_type_alias
	);
	free_ht(
		state->function_decls,
		(void(*)(void *))free_function_declaration
	);
	reset_scope_head();
	free_scope(state->scope);
}
