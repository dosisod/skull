#include <string.h>

#include "skull/codegen/c/shared.h"

SkullStateC SKULL_STATE_C;

#define MAX_TABS 64

char *get_indentation(void) {
	// use a statically allocated buffer to store tabs, since this will be
	// called a lot. Buffer is only 64 chars since indentation won't be
	// of much use in a program that complex.
	static char tabs[MAX_TABS] = {0};
	memset(tabs, '\t', MAX_TABS);

	if (SKULL_STATE_C.indent_lvl < MAX_TABS) {
		tabs[SKULL_STATE_C.indent_lvl] = '\0';
	}

	return tabs;
}
