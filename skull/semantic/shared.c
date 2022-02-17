#include <stdbool.h>

#include "skull/build_data.h"
#include "skull/common/malloc.h"
#include "skull/semantic/func.h"
#include "skull/semantic/scope.h"
#include "skull/semantic/symbol.h"

#include "skull/semantic/shared.h"

SemanticState SEMANTIC_STATE;

char *create_main_func_name(const char *);
void free_dynamic_type(void *);

void setup_semantic_state(void) {
	FunctionDeclaration *func = Calloc(1, sizeof(FunctionDeclaration));
	*func = (FunctionDeclaration){
		.return_type = &TYPE_INT
	};

	char *func_name = create_main_func_name(BUILD_DATA.filename);

	Symbol *symbol = Calloc(1, sizeof(FunctionDeclaration));
	symbol->func = func;
	symbol->name = func_name;
	symbol->linkage_name = func_name;

	SEMANTIC_STATE.main_func = symbol;
	SEMANTIC_STATE.current_func = SEMANTIC_STATE.main_func;
	SEMANTIC_STATE.last_func = NULL;
}

/*
Free everything about a Skull semantic instance.
*/
void free_semantic_state(void) {
	SemanticState *state = &SEMANTIC_STATE;

	reset_scope_head();
	free_scope(state->scope);

	if (state->main_func) {
		state->main_func->func->was_called = true;
		free_function_declaration(state->main_func);
	}

	state->scope = NULL;
	state->while_loop_depth = 0;

	free_ht(SEMANTIC_STATE.dynamic_refs, free_dynamic_type);
	SEMANTIC_STATE.dynamic_refs = NULL;
}
