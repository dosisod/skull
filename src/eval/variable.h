#pragma once

#include <stddef.h>
#include <stdlib.h>

#include "../parse/tokenize.h"
#include "../parse/types.h"

typedef struct variable_t {
	const wchar_t *type;
	const wchar_t *name;
	size_t bytes;
	char *mem;
} variable_t;

/*
Make a variable called `name` with type `type`.

Returns NULL if var cannot be created, else pointer to created var.
*/
variable_t *make_variable(const wchar_t *type, const wchar_t *name) {
	variable_t *var=malloc(sizeof(variable_t));

	type_t *current=&TYPES_AVAILABLE;
	while (current) {
		if (wcscmp(current->name, type)==0) {
			break;
		}

		current=current->next;
	}
	if (current==NULL) {
		free(var);
		return NULL;
	}

	var->name=name;
	var->type=type;
	var->bytes=current->bytes;

	char *mem=calloc(current->bytes, sizeof(char));
	var->mem=mem;

	return var;
}

/*
Free a given `variable_t` variable.
*/
void free_variable_t(variable_t *var) {
	free(var->mem);
	free(var);
}