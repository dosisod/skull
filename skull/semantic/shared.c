#include "skull/codegen/scope.h"
#include "skull/common/malloc.h"
#include "skull/semantic/func.h"

#include "skull/semantic/shared.h"

SemanticState SEMANTIC_STATE;

/*
Free everything about a Skull compiler instance.
*/
void free_state_semantic(void) {
	SemanticState *state = &SEMANTIC_STATE;

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
