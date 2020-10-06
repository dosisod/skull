#include <stddef.h>
#include <stdlib.h>

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
Variable *scope_find_name(const Scope *scope, const char32_t *name) {
	if (!scope) {
		return NULL;
	}

	size_t tmp = 0;
	while (tmp < scope->vars_used) {
		if (c32scmp(scope->vars[tmp]->name, name)) {
			return scope->vars[tmp];
		}
		tmp++;
	}
	return NULL;
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
	size_t tmp = 0;

	while (tmp < scope->vars_used) {
		free_variable(scope->vars[tmp]);
		tmp++;
	}

	free(scope);
}
