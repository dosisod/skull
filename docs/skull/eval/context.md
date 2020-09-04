# skull/eval/context.c

```c
bool context_add_var(Context *ctx, Variable *var)
```

> Add variable `var` to context `ctx`.
> \
> Returns `true` if `var` was added, else `false`

```c
Variable *context_find_name(const Context *ctx, const char32_t *name)
```

> Returns pointer to variable with matching `name` if found, else `NULL`

```c
Context *make_context(void)
```

> Returns a new variable context.

```c
void free_context(Context *ctx)
```

> Frees a context `ctx` and all the variables inside of it.

