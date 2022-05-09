#include <stdbool.h>

#include "skull/build_data.h"
#include "skull/common/malloc.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/symbol.h"

#include "skull/semantic/shared.h"

char *create_main_func_name(const char *);
void free_dynamic_type(void *);

SemanticState *setup_semantic_state(void) {
	FunctionDeclaration *func = Calloc(1, sizeof(FunctionDeclaration));
	*func = (FunctionDeclaration){
		.return_type = &TYPE_INT
	};

	char *func_name = create_main_func_name(BUILD_DATA.filename);

	Symbol *symbol = Calloc(1, sizeof(FunctionDeclaration));
	symbol->func = func;
	symbol->name = func_name;
	symbol->linkage_name = func_name;

	SemanticState *state = Calloc(1, sizeof(SemanticState));

	state->main_func = symbol;
	state->current_func = state->main_func;
	state->last_func = NULL;

	return state;
}

/*
Free everything about a Skull semantic instance.
*/
void free_semantic_state(SemanticState *state) {
	if (!state) return;

	reset_scope_head(state);
	free_scope(state->scope);

	if (state->main_func) {
		state->main_func->func->was_called = true;
		free_function_declaration(state->main_func);
	}

	free_ht(state->dynamic_refs, free_dynamic_type);

	*state = (SemanticState){0};
	free(state);
}
