# skull/eval/context.c

```c
bool context_add_var(context_t *ctx, variable_t *var)
```

> Add variable `var` to context `ctx`.
> \
> Returns `true` if `var` was added, else `false`

```c
variable_t *context_find_name(const context_t *ctx, const char32_t *name)
```

> Returns pointer to variable with matching `name` if found, else `NULL`

```c
context_t *make_context(void)
```

> Returns a new variable context.

```c
void free_context(context_t *ctx)
```

> Frees a context `ctx` and all the variables inside of it.

