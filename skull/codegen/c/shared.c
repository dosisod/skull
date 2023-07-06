#include <stdlib.h>
#include <string.h>

#include "skull/common/malloc.h"

#include "skull/codegen/c/shared.h"

enum { MAX_TABS = 64 };

SkullStateC *setup_c_state(SemanticState *state) {
	(void)state;

	return Calloc(1, sizeof(SkullStateC));
}

char *get_indentation(const SkullStateC *state) {
	// use a statically allocated buffer to store tabs, since this will be
	// called a lot. Buffer is only 64 chars since indentation won't be
	// of much use in a program that complex.
	static char tabs[MAX_TABS] = {0};
	memset(tabs, '\t', MAX_TABS);

	const unsigned max = state->indent_lvl >= MAX_TABS ?
		MAX_TABS - 1 :
		state->indent_lvl;

	tabs[max] = '\0';

	return tabs;
}

void free_c_state(SkullStateC *state) {
	if (!state) return;

	free(state->globals);
	free(state->tree);
	state->globals = NULL;
	state->tree= NULL;

	free(state);
}
