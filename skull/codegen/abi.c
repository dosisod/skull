#include <string.h>

#include "skull/common/malloc.h"

#include "skull/codegen/abi.h"

/*
Convert/mangle `filename` into suitable name for "main" method for module.
*/
char *create_main_func_name(const char *filename) {
	if (!filename) filename = "";

	char *slash_pos = strrchr(filename, '/');

	if (slash_pos) {
		filename = slash_pos + 1;
	}

	const size_t len = *filename ? strlen(filename) - 1 : 2;

	char *ret = Malloc(len);
	ret[0] = '.';
	strncpy(ret + 1, filename, len - 1);
	ret[len - 1] = '\0';

	return ret;
}
