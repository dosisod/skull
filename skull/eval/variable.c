#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/errors.h"
#include "skull/parse/classify.h"

#include "skull/eval/variable.h"

/*
Return string representation of the variable `var`.

The result of this function must be freed.
*/
char32_t *fmt_var(const variable_t *var) {
	if (!var->type || !var->type->to_string) {
		return NULL;
	}
	return var->type->to_string(var);
}

/*
Read variable memory of `var` into `dest`.
*/
void variable_read(void *dest, const variable_t *var) {
	memcpy(dest, var->mem, var->bytes);
}

/*
Make a variable called `name` with type `type`, and make it const if `is_const` is true.

Returns `NULL` if var cannot be created, else pointer to created var.
*/
variable_t *make_variable(const char32_t *type, const char32_t *name, bool is_const) {
	type_t *found_type = find_type(type);

	if (!found_type || !is_valid_identifier_str(name)) {
		return NULL;
	}

	variable_t *var;
	var = malloc(sizeof *var);
	DIE_IF_MALLOC_FAILS(var);

	const size_t len = c32slen(name);
	char32_t *name_copy;
	name_copy = malloc((len + 1) * sizeof *name_copy);
	DIE_IF_MALLOC_FAILS(name_copy);

	c32slcpy(name_copy, name, len + 1);

	var->name=name_copy;
	var->type=found_type;
	var->is_const=is_const;
	var->bytes=found_type->bytes;

	uint8_t *mem;
	mem = calloc(found_type->bytes, sizeof *mem);
	DIE_IF_MALLOC_FAILS(mem);

	var->mem = mem;
	return var;
}

/*
Write `data` to `var`.

If `var` is constant, return error msg, else `NULL`.
*/
const char32_t *variable_write(const variable_t *var, const void *data) {
	if (var->is_const) {
		return FMT_ERROR(ERR_CANNOT_ASSIGN_CONST, { .var = var });
	}

	memcpy(var->mem, data, var->bytes);
	return NULL;
}

/*
Free variable `var` and its internal memory.
*/
void free_variable(variable_t *var) {
	if (var) {
		free((char32_t*)var->name);
		free(var->mem);
		free(var);
	}
}