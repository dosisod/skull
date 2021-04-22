#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/compiler/types/defs.h"
#include "skull/parse/classify.h"

#include "skull/compiler/variable.h"

/*
Make a variable called `name` with type `type`, and make it const if
`is_const` is true.

Returns `NULL` if var cannot be created, else pointer to created var.
*/
Variable *make_variable(
	Type type,
	const char32_t *const name,
	bool is_const
) {
	if (!type || !is_valid_identifier_str(name)) return NULL;

	Variable *var;
	var = Malloc(sizeof *var);

	*var = (Variable){
		.name = c32stombs(name),
		.type = type,
		.is_const = is_const,
		.is_const_lit = false,
		.is_global = false,
		.ref = NULL
	};

	return var;
}

extern bool SKULL_TESTING;

/*
Free variable `var`.
*/
void free_variable(Variable *var) {
	if (var) {
		if (!SKULL_TESTING) {
			if (!var->is_const && !var->was_reassigned) {
				FMT_WARN(WARN_VAR_NOT_CONST, { .var = var });
			}
			if (!var->was_read) {
				FMT_WARN(WARN_VAR_UNUSED, { .var = var });
			}
		}

		free(var->name);
		free(var);
	}
}
