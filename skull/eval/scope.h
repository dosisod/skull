#pragma once

#include "skull/eval/variable.h"

#define SCOPE_MAX_VARS 256

/*
`Scope` stores all of the variables declared in a specific scope.

`vars_used` stores how many variables are in a given scope.

`vars` is an array of all of the variables in a scope.
*/
typedef struct {
	size_t vars_used;
	Variable *vars[SCOPE_MAX_VARS];
} Scope;

Scope *make_scope(void);

Variable *scope_find_name(const Scope *, const char32_t *);
_Bool scope_add_var(Scope *const, Variable *);

void free_scope(Scope *);
