#include "skull/common/malloc.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"

#include "skull/semantic/shared.h"

SemanticState SEMANTIC_STATE;

/*
Free everything about a Skull compiler instance.
*/
void free_semantic_state(void) {
	SemanticState *state = &SEMANTIC_STATE;

	reset_scope_head();
	free_scope(state->scope);
}
