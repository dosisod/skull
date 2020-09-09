#pragma once

#include "skull/eval/variable.h"

#define CONTEXT_MAX_VARS 256

typedef struct Context {
	struct Context *child;

	size_t vars_used;
	Variable *vars[CONTEXT_MAX_VARS];
} Context;

Context *make_context(void);

Variable *context_find_name(const Context *, const char32_t *);
_Bool context_contains_var(Context *, const Variable *);
_Bool context_add_var(Context *, Variable *);

void free_context(Context *);
