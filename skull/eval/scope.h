#pragma once

#include "skull/eval/variable.h"

#define SCOPE_MAX_VARS 256

typedef struct Scope {
	struct Scope *child;

	size_t vars_used;
	Variable *vars[SCOPE_MAX_VARS];
} Scope;

Scope *make_scope(void);

Variable *scope_find_name(const Scope *, const char32_t *);
_Bool scope_contains_var(Scope *, const Variable *);
_Bool scope_add_var(Scope *, Variable *);

void free_scope(Scope *);
