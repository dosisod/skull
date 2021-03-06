#include <stdbool.h>
#include <stddef.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/str.h"
#include "skull/parse/classify.h"

#include "skull/semantic/variable.h"

extern bool SKULL_TESTING;

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
		.name = c32stombs(name, NULL),
		.type = type,
		.is_const = is_const
	};

	return var;
}

/*
Free variable `var`.
*/
void free_variable(Variable *var) {
	if (!var) return;

	if (!SKULL_TESTING) {
		if (!var->is_const && !var->was_reassigned)
			FMT_WARN(WARN_VAR_NOT_CONST, {
				.var = var, .loc = &var->location
			});

		if (!var->was_read && !var->is_exported)
			FMT_WARN(WARN_VAR_UNUSED, {
				.var = var, .loc = &var->location
			});
	}

	free(var->name);
	free(var);
}

/*
Disable warnings on variable `var`.
*/
void variable_no_warnings(Variable *var) {
	var->was_reassigned = true;
	var->was_read = true;
}
