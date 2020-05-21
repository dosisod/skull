# src/eval/context.h

#### `context_t *make_context()`
Returns a new variable context.

#### `bool context_contains_var(context_t *ctx, const variable_t *var)`
Returns true if `var` is defined in a given context `ctx`.

#### `void context_add_var(context_t *ctx, variable_t *var)`
Add variable `var` to context `ctx`.

#### `void context_add_ctx(context_t *ctx, context_t *ctx_new)`
Add another nested context to an existing context.

#### `void free_context(context_t *ctx)`
Frees a context `ctx` and all the variables inside of it.

