#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/malloc.h"
#include "skull/common/str.h"

#include "skull/eval/scope.h"

/*
Add variable `var` to `scope`.

Returns `true` if `var` was added, else `false`
*/
bool scope_add_var(Scope *const scope, Variable *var) {
	if (scope_find_name(scope, var->name)) {
		return false;
	}

	scope->vars[scope->vars_used] = var;
	scope->vars_used++;

	return true;
}

/*
Returns pointer to variable with matching `name` if found, else `NULL`
*/
Variable *scope_find_name(const Scope *scope, const char *name) {
	if (!scope) {
		return NULL;
	}

	size_t var_at = 0;
	while (var_at < scope->vars_used) {
		if (strcmp(scope->vars[var_at]->name, name) == 0) {
			return scope->vars[var_at];
		}
		var_at++;
	}

	return scope_find_name(scope->sub_scope, name);
}

/*
Returns a new variable scope.
*/
Scope *make_scope(void) {
	Scope *scope;
	scope = calloc(1, sizeof *scope);
	DIE_IF_MALLOC_FAILS(scope);

	return scope;
}

/*
Frees a `scope` and all the variables inside of it.
*/
void free_scope(Scope *scope) {
	size_t var_at = 0;
	while (var_at < scope->vars_used) {
		free_variable(scope->vars[var_at]);
		var_at++;
	}

	free(scope);
}
