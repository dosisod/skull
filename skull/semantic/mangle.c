#include <stdlib.h>
#include <string.h>

#include "skull/build_data.h"
#include "skull/common/malloc.h"

#include "skull/semantic/mangle.h"

char *mangle_name(const char *name) {
	const char *module_name = strrchr(BUILD_DATA.filename, '/');
	if (module_name) module_name++;
	else module_name = BUILD_DATA.filename;

	const size_t module_len = strlen(module_name) - 2;
	const size_t name_len = strlen(name);

	char *new_name = Malloc(module_len + name_len + 2);

	strncpy(new_name, module_name, module_len);
	new_name[module_len] = '.';
	strncpy(new_name + module_len, name, name_len);
	new_name[module_len + name_len] = '\0';

	return new_name;
}
