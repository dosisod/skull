# src/eval/context.h

#### `context_t *make_context()`
Returns a new variable context.

#### `void context_add(context_t *ctx, variable_t *var)`
Add variable `var` to context `ctx`.

#### `void free_context(context_t *ctx)`
Frees a context `ctx` and all the variables inside of it.

