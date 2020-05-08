#pragma once

typedef struct variable_t {
	const char *type;
	const char *name;
	size_t bytes;
	char *mem;
} variable_t;

/*
Make a variable called `name` with type `type`, allocating `bytes` amount of bytes.

Returns NULL if var cannot be created, else pointer to created var.
*/
variable_t *make_variable(const char *type, const char *name, size_t bytes) {
	variable_t *var=malloc(sizeof(variable_t));

	bool found=false;
	type_t *current=&TYPES_AVAILABLE;
	while (current) {
		if (strcmp(current->name, type)==0) {
			found=true;
			break;
		}
		current=current->next;
	}
	if (!found) return NULL;

	var->name=name;
	var->type=type;
	var->bytes=bytes;

	char *mem=calloc(bytes, sizeof(char));
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