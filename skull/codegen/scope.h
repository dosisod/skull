#pragma once

#include "skull/codegen/shared.h"
#include "skull/common/hashtable.h"
#include "skull/compiler/variable.h"
#include "skull/parse/token.h"

#include "skull/compiler/variable.h"

typedef struct Scope Scope;

/*
`Scope` stores all of the variables declared in a specific scope.

`vars` is an array of all of the variables in a scope.

`child` stores a nested scope with more variables in it.
*/
typedef struct Scope {
	HashTable *vars;

	Scope *next;
	Scope *last;
	Scope *child;
	Scope *parent;
} Scope;

Scope *make_scope(void);

Variable *scope_find_name(const Scope *const, const char *);
_Bool scope_add_var(Scope **, Variable *const);

void free_scope(Scope *);

void make_child_scope(void);
void restore_parent_scope(void);
void make_adjacent_scope(void);
Scope *find_scope_head(void);
void reset_scope_head(void);
_Bool is_top_lvl_scope(void);

Variable *scope_find_var(const Token *const, _Bool);
