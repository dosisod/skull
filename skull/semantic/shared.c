#include <stdbool.h>

#include "skull/build_data.h"
#include "skull/common/malloc.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"

#include "skull/semantic/shared.h"

SemanticState SEMANTIC_STATE;

char *create_main_func_name(const char *);

void setup_semantic_state(void) {
	SEMANTIC_STATE.main_func = Calloc(1, sizeof(FunctionDeclaration));

	*SEMANTIC_STATE.main_func = (FunctionDeclaration){
		.name = create_main_func_name(BUILD_DATA.filename),
		.return_type = TYPE_INT
	};

	SEMANTIC_STATE.current_func = SEMANTIC_STATE.main_func;
	SEMANTIC_STATE.last_func = NULL;
}

/*
Free everything about a Skull compiler instance.

Return `true` if an error occurred.
*/
_Bool free_semantic_state(void) {
	SemanticState *state = &SEMANTIC_STATE;

	reset_scope_head();
	const bool failed = free_scope(state->scope);

	if (state->main_func) {
		state->main_func->was_called = true;
		free_function_declaration(state->main_func);
	}

	state->scope = NULL;
	state->while_loop_depth = 0;

	return failed;
}
