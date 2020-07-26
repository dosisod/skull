# skull/eval/context.c

#### `bool context_add_var(context_t *ctx, variable_t *var)`
Add variable `var` to context `ctx`.

Returns `true` if `var` was added, else `false`

#### `variable_t *context_find_name(const context_t *ctx, const char32_t *name)`
Returns pointer to variable with matching `name` if found, else `NULL`

#### `context_t *make_context(void)`
Returns a new variable context.

#### `void context_add_ctx(context_t *ctx, context_t *ctx_new)`
Add another nested context to an existing context.

#### `void free_context(context_t *ctx)`
Frees a context `ctx` and all the variables inside of it.

