#pragma once

#include "variable.h"

#define CONTEXT_MAX_VARS 256

typedef struct context_t {
	struct context_t *child;
	struct context_t *parent;

	size_t vars_used;
	variable_t *vars[CONTEXT_MAX_VARS];
} context_t;

/*
Returns a new variable context.
*/
context_t *make_context(void);

/*
Returns pointer to variable with matching `name` if found, else `NULL`
*/
variable_t *context_find_name(const context_t *ctx, const char32_t *name);

/*
Returns true if `var` is defined in a given context `ctx`.
*/
bool context_contains_var(context_t *ctx, const variable_t *var);

/*
Add variable `var` to context `ctx`.

Returns `true` if `var` was added, else `false`
*/
bool context_add_var(context_t *ctx, variable_t *var);

/*
Add another nested context to an existing context.
*/
void context_add_ctx(context_t *ctx, context_t *ctx_new);

/*
Frees a context `ctx` and all the variables inside of it.
*/
void free_context(context_t *ctx);
