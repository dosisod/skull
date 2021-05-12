#include "skull/compiler/types/_underscore_num.h"

/*
Strip underscores from `str`.

This function modifies the contents at `str`.

If `c` is defined, stop iterating when `c` is found.
*/
void strip_underscore_num(char *str, char c) {
	char *probe = str;

	while (*probe && *probe != c) {
		if (*probe == '_') probe++;

		*str = *probe;
		str++;
		probe++;
	}

	// copy rest of string if `c` was hit
	while (*probe) {
		*str = *probe;
		str++;
		probe++;
	}
	*str = '\0';
}
