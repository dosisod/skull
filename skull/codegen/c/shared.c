#include <stdlib.h>
#include <string.h>

#include "skull/codegen/c/shared.h"

static SkullStateC SKULL_STATE_C;

#define MAX_TABS 64

SkullStateC *setup_c_state(void) {
	return &SKULL_STATE_C;
}

char *get_indentation(const SkullStateC *state) {
	// use a statically allocated buffer to store tabs, since this will be
	// called a lot. Buffer is only 64 chars since indentation won't be
	// of much use in a program that complex.
	static char tabs[MAX_TABS] = {0};
	memset(tabs, '\t', MAX_TABS);

	if (state->indent_lvl < MAX_TABS) {
		tabs[state->indent_lvl] = '\0';
	}

	return tabs;
}

void free_c_state(void) {
	free(SKULL_STATE_C.globals);
	SKULL_STATE_C.globals = NULL;
}
