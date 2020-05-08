#pragma once

typedef struct variable_t {
	const char *type;
	const char *name;
	size_t bytes;
	char *mem;
} variable_t;

/*
Returns a variable called `name` with type `type`, allocating `bytes` amount of bytes.
*/
variable_t *make_variable(const char *type, const char *name, size_t bytes) {
	variable_t *var=malloc(sizeof(variable_t));

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