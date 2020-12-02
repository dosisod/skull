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

	var->name = c32stombs(name);
	var->type = type;
	var->is_const = is_const;
	var->is_global = false;
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