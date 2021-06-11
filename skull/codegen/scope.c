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
	if (!scope || !(scope->vars || scope->parent || scope->last)) return NULL;

	if (scope->vars) {
		Variable *var = ht_get(scope->vars, name);
		if (var) return var;
	}

	if (scope->parent) return scope_find_name(scope->parent, name);
	if (scope->last) return scope_find_name(scope->last, name);

	return NULL;
}

/*
Returns a new variable scope.
*/
Scope *make_scope(void) {
	return Calloc(1, sizeof(Scope));
}

/*
Try and find a variable stored in `token`.

Allow for finding of uninitialized variables by setting `allow_uninitialized`.

Return `NULL` if variable was not found.
*/
Variable *scope_find_var(const Token *const token, bool allow_uninitialized) {
	char *const var_name = token_mbs_str(token);
	Variable *const var = scope_find_name(SKULL_STATE.scope, var_name);

	if (!var || (!allow_uninitialized && !var->ref)) {
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

	scope->child = child;
	child->parent = scope;
	SKULL_STATE.scope = child;
}

/*
Make a scope adjacent to the current scope.

An adjacent scope is one that is on the same level as the current scope and
after a child scope.
*/
void make_adjacent_scope(void) {
	Scope *next = make_scope();
	next->last = SKULL_STATE.scope;
	SKULL_STATE.scope->next = next;

	SKULL_STATE.scope = next;
}

/*
Find the head of the current scope (without going to the parent node).
*/
Scope *find_scope_head(void) {
	if (!SKULL_STATE.scope) return NULL;

	Scope *last = SKULL_STATE.scope->last;
	if (!last) return SKULL_STATE.scope;

	while (last && last->last) {
		last = last->last;
	}

	return last;
}

bool is_top_lvl_scope(void) {
	Scope *head = find_scope_head();

	return !head || !head->parent;
}

/*
Move the current scope to the scope head.
*/
void reset_scope_head(void) {
	if (!SKULL_STATE.scope) return;

	SKULL_STATE.scope = find_scope_head();
}

/*
Free current scope, set current scope to parent scope.
*/
void restore_parent_scope(void) {
	if (!SKULL_STATE.scope) return;

	reset_scope_head();
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
