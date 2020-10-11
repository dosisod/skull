#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/eval/types/defs.h"
#include "skull/parse/classify.h"

#include "skull/eval/variable.h"

/*
Return string representation of the variable `var`.

The result of this function must be freed.
*/
char *fmt_var(const Variable *const var) {
	if (!var->type || !var->type->to_string) {
		return NULL;
	}
	return var->type->to_string(var);
}

/*
Read variable memory of `var` into `dest`.
*/
void variable_read(void *const dest, const Variable *const var) {
	memcpy(dest, var->mem, var->type->bytes);
}

/*
Make a variable called `name` with type `type`, and make it const if `is_const` is true.

Returns `NULL` if var cannot be created, else pointer to created var.
*/
Variable *make_variable(const Type *const type, const char32_t *const name, bool is_const) {
	if (!type || !is_valid_identifier_str(name)) {
		return NULL;
	}

	Variable *var;
	var = malloc(sizeof *var);
	DIE_IF_MALLOC_FAILS(var);

	const size_t len = c32slen(name);
	char32_t *name_copy;
	name_copy = malloc((len + 1) * sizeof *name_copy);
	DIE_IF_MALLOC_FAILS(name_copy);

	c32sncpy(name_copy, name, len + 1);
	name_copy[len] = '\0';

	var->name = name_copy;
	var->type = type;
	var->is_const = is_const;
	var->alloca = NULL;

	unsigned char *mem;
	mem = calloc(type->bytes, sizeof *mem);
	DIE_IF_MALLOC_FAILS(mem);

	var->mem = mem;
	return var;
}

/*
Write `data` to `var`.

If `var` is constant, return error msg, else `NULL`.
*/
const char32_t *variable_write(const Variable *const var, const void *const data) {
	if (var->is_const) {
		return FMT_ERROR(ERR_CANNOT_ASSIGN_CONST, { .var = var });
	}

	memcpy(var->mem, data, var->type->bytes);
	return NULL;
}

/*
Free variable `var` and its internal memory.
*/
void free_variable(Variable *var) {
	if (var) {
		if (var->type == &TYPE_STR) {
			SkullStr str_mem = NULL;
			variable_read(&str_mem, var);
			free(str_mem);
		}
		free((char32_t *)var->name);
		free(var->mem);
		free(var);
	}
}