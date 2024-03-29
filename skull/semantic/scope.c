#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/semantic/func.h"
#include "skull/semantic/shared.h"
#include "skull/semantic/symbol.h"

#include "skull/semantic/scope.h"


static void free_ht_symbol(HashItem *);
static void free_symbol(Symbol *);
static Scope *find_scope_head(SemanticState *);

/*
Add `symbol` (as variable) to current scope.

Returns `true` if `var` was added, else `false`
*/
bool scope_add_var(SemanticState *state, Symbol *symbol) {
	if (
		!state ||
		(state->scope && scope_find_name(state->scope, symbol->name))
	) {
		return false;
	}

	return scope_add_symbol(state, symbol);
}

/*
Returns pointer to variable with matching `name` if found, else `NULL`

This function might modify the contents of `name` durring execution, but will
always keep the data the same after the function has finished.
*/
Symbol *scope_find_name(const Scope *const scope, char *name) {
	if (!scope || !(scope->symbols || scope->parent || scope->last)) {
		return NULL;
	}

	if (scope->symbols) {
		char *dot_position = strchr(name, '.');

		if (dot_position) {
			*dot_position = '\0';

			Symbol *namespace = scope_find_name(scope, name);
			Symbol *symbol = NULL;

			if (namespace && namespace->type == SYMBOL_NAMESPACE) {
				symbol = scope_find_name(namespace->scope, dot_position + 1);
			}

			*dot_position = '.';

			return symbol;
		}

		Symbol *symbol = ht_get(scope->symbols, name);
		if (symbol) return symbol;
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
Symbol *scope_find_var(SemanticState *state, const Token *const token) {
	char *const var_name = token_to_mbs_str(token);
	Symbol *const symbol = scope_find_name(state->scope, var_name);

	if (!symbol ||
		!symbol->var ||
		!symbol->var->is_defined
	) {
		FMT_ERROR(ERR_VAR_NOT_FOUND, {
			.loc = &token->location, .real = var_name
		});

		return NULL;
	}
	free(var_name);

	return symbol;
}

/*
Add a child scope to the current and replace current scope with new one.
*/
void make_child_scope(SemanticState *state) {
	if (!state) return;

	Scope *scope = state->scope;
	if (!scope) scope = make_scope();

	Scope *child = scope->child ? scope->child : make_scope();

	scope->child = child;
	child->parent = scope;
	state->scope = child;
}

/*
Make a scope adjacent to the current scope.

An adjacent scope is one that is on the same level as the current scope and
after a child scope.
*/
void make_adjacent_scope(SemanticState *state) {
	if (!state) return;

	Scope *next = make_scope();
	next->last = state->scope;
	state->scope->next = next;

	state->scope = next;
}

/*
Find the head of the current scope (without going to the parent node).
*/
static Scope *find_scope_head(SemanticState *state) {
	if (!state || !state->scope) return NULL;

	Scope *last = state->scope->last;
	if (!last) return state->scope;

	while (last && last->last) {
		last = last->last;
	}

	return last;
}

bool is_top_lvl_scope(SemanticState *state) {
	Scope *head = find_scope_head(state);

	return !head || !head->parent;
}

/*
Move the current scope to the scope head.
*/
void reset_scope_head(SemanticState *state) {
	if (!state || !state->scope) return;

	state->scope = find_scope_head(state);
}

/*
Free current scope, set current scope to parent scope.
*/
void restore_parent_scope(SemanticState *state) {
	if (!state || !state->scope) return;

	reset_scope_head(state);
	state->scope = state->scope->parent;
}

static void free_ht_symbol(HashItem *item) {
	free_symbol(item->data);
}

static void free_symbol(Symbol *symbol) {
	if (!symbol) return;

	if (symbol->type == SYMBOL_VAR) {
		free_variable(symbol->var);
		free(symbol);
	}
	else if (symbol->type == SYMBOL_FUNC) {
		free_function_declaration(symbol);
	}
	else if (symbol->type == SYMBOL_ALIAS || symbol->type == SYMBOL_NAMESPACE) {
		free(symbol->name);
		free(symbol);
	}
}

/*
Frees a `scope` and all the symbols inside of it.
*/
void free_scope(Scope *scope) {
	if (!scope) return;

	if (scope->symbols) {
		free_ht(scope->symbols, (void(*)(void *))free_ht_symbol);
		scope->symbols = NULL;
	}

	if (scope->child) free_scope(scope->child);
	if (scope->next) free_scope(scope->next);

	free(scope);
}

Scope *get_last_adjacent_scope(Scope *scope) {
	while (scope && scope->next) {
		scope = scope->next;
	}

	return scope;
}
