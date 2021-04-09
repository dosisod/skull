#pragma once

#include "skull/common/hashtable.h"

#include "skull/compiler/variable.h"

typedef struct Scope Scope;

/*
`Scope` stores all of the variables declared in a specific scope.

`vars_used` stores how many variables are in a given scope.

`vars` is an array of all of the variables in a scope.

`sub_scope` stores a nested scope with more variables in it.
*/
typedef struct Scope {
	HashTable *vars;

	Scope *sub_scope;
} Scope;

Scope *make_scope(void);

Variable *scope_find_name(const Scope *const, const char *);
_Bool scope_add_var(Scope *const, Variable *const);

void free_scope(Scope *);