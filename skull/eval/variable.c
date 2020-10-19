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

	return var;
}

/*
Free variable `var`.
*/
void free_variable(Variable *var) {
	if (var) {
		free(var->name);
		free(var);
	}
}