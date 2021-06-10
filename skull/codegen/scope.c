#include <stdbool.h>

#include "skull/codegen/shared.h"
#include "skull/common/errors.h"
#include "skull/common/malloc.h"

#include "skull/codegen/scope.h"


static void free_ht_variable(HashItem *);

/*
Add variable `var` to `scope`.

Returns `true` if `var` was added, else `false`
*/
bool scope_add_var(Scope **scope, Variable *const var) {
	if (*scope && scope_find_name(*scope, var->name)) return false;

	if (!*scope) *scope = make_scope();
	if (!(*scope)->vars) (*scope)->vars = ht_create();

	return ht_add((*scope)->vars, var->name, var);
}

/*
Returns pointer to variable with matching `name` if found, else `NULL`
*/
Variable *scope_find_name(const Scope *const scope, const char *name) {
	if (!scope || (!scope->vars && !scope->parent)) return NULL;

	if (!scope->vars && scope->parent)
		return scope_find_name(scope->parent, name);

	Variable *var = ht_get(scope->vars, name);
	if (var) return var;

	return scope_find_name(scope->parent, name);
}

/*
Returns a new variable scope.
*/
Scope *make_scope(void) {
	return Calloc(1, sizeof(Scope));
}

/*
Try and find a variable stored in `token`.

Return `NULL` if variable was not found.
*/
Variable *scope_find_var(const Token *const token) {
	char *const var_name = token_mbs_str(token);
	Variable *const var = scope_find_name(SKULL_STATE.scope, var_name);

	if (!var || !var->ref) {
		FMT_ERROR(ERR_VAR_NOT_FOUND, {
			.loc = &token->location, .real = var_name
		});

		return NULL;
	}
	free(var_name);

	return var;
}

/*
Add a child scope to the current and replace current scope with new one.
*/
void make_child_scope(void) {
	Scope *scope = SKULL_STATE.scope;
	if (!scope) scope = make_scope();

	Scope *child = make_scope();

	if (scope->child) {
		free_scope(scope->child);
	}

	scope->child = child;
	child->parent = scope;
	SKULL_STATE.scope = child;
}

/*
Free current scope, set current scope to parent scope.
*/
void restore_parent_scope(void) {
	SKULL_STATE.scope = SKULL_STATE.scope->parent;
}

static void free_ht_variable(HashItem *item) {
	if (item->data) free_variable(item->data);
}

/*
Frees a `scope` and all the variables inside of it.
*/
void free_scope(Scope *scope) {
	if (scope) {
		if (scope->vars)
			free_ht(scope->vars, (void(*)(void *))free_ht_variable);

		if (scope->child) free_scope(scope->child);
		if (scope->next) free_scope(scope->next);

		free(scope);
	}
}
