#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "skull/common/malloc.h"
#include "skull/common/range.h"
#include "skull/common/str.h"

#include "skull/eval/scope.h"

/*
Add variable `var` to `scope`.

Returns `true` if `var` was added, else `false`
*/
bool scope_add_var(Scope *const scope, Variable *const var) {
	if (scope_find_name(scope, var->name)) return false;

	return ht_add(scope->vars, var->name, var);
}

/*
Returns pointer to variable with matching `name` if found, else `NULL`
*/
Variable *scope_find_name(const Scope *const scope, const char *name) {
	if (!scope) return NULL;

	Variable *var = ht_get(scope->vars, name);
	if (var) return var;

	return scope_find_name(scope->sub_scope, name);
}

/*
Returns a new variable scope.
*/
Scope *make_scope(void) {
	Scope *scope;
	scope = Calloc(1, sizeof *scope);
	scope->vars = ht_create();

	return scope;
}

void free_ht_variable(HashItem *item) {
	if (item->data) free_variable(item->data);
}

/*
Frees a `scope` and all the variables inside of it.
*/
void free_scope(Scope *scope) {
	free_ht(scope->vars, (void(*)(void *))free_ht_variable);

	free(scope);
}
