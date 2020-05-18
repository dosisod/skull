#pragma once

#include <stddef.h>
#include <stdlib.h>

#include "../parse/tokenize.h"
#include "../parse/types.h"

#define VARIABLE_WRITE_OK 0
#define VARIABLE_WRITE_ECONST 1

typedef struct variable_t {
	const wchar_t *type;
	const wchar_t *name;
	bool is_const;

	size_t bytes;
	uint8_t *mem;
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

	uint8_t *mem=calloc(current->bytes, sizeof(uint8_t));
	var->mem=mem;

	return var;
}

/*
Write `data` to `var`.

If `var` is const, `VARIABLE_WRITE_ECONST` is returned.
Upon success, `VARIABLE_WRITE_OK` is returned.
*/
uint8_t variable_write(const variable_t *var, const void *data) {
	if (var->is_const) {
		return VARIABLE_WRITE_ECONST;
	}

	memcpy(var->mem, data, var->bytes);
	return VARIABLE_WRITE_OK;
}

/*
Read variable `var`s memory into `dest`.
*/
void variable_read(void *dest, const variable_t *var) {
	memcpy(dest, var->mem, var->bytes);
}

/*
Free a given `variable_t` variable.
*/
void free_variable_t(variable_t *var) {
	free(var->mem);
	free(var);
}