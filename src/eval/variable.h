#pragma once

#include "../parse/types.h"

typedef struct variable_t {
	type_t *type;
	const char32_t *name;
	bool is_const;

	size_t bytes;
	uint8_t *mem;
} variable_t;

/*
Make a variable called `name` with type `type`, and make it const if `is_const` is true.

Returns `NULL` if var cannot be created, else pointer to created var.
*/
variable_t *make_variable(const char32_t *type, const char32_t *name, bool is_const);

/*
Write `data` to `var`.

If `var` is constant, return error msg, else `NULL`.
*/
const char32_t *variable_write(const variable_t *var, const void *data);

/*
Read variable `var`s memory into `dest`.
*/
void variable_read(void *dest, const variable_t *var);

/*
Free a given `variable_t` variable.
*/
void free_variable(variable_t *var);

/*
Return string representation of a variable.

The result of this function must be freed.
*/
char32_t *fmt_var(const variable_t *var);