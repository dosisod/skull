#pragma once

#include <stddef.h>
#include <stdlib.h>

#include "../parse/tokenize.h"
#include "../parse/types.h"

typedef struct variable_t {
	const wchar_t *type;
	const wchar_t *name;
	bool is_const;
	size_t bytes;
	char *mem;
} variable_t;

/*
Make a variable called `name` with type `type`, and make it const if `is_const` is true.

Returns NULL if var cannot be created, else pointer to created var.
*/
variable_t *make_variable(const wchar_t *type, const wchar_t *name, bool is_const) {
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
	var->is_const=is_const;
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