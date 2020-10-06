# skull/eval/scope.c

```c
bool scope_add_var(Scope *const scope, Variable *var)
```

> Add variable `var` to `scope`.
> \
> Returns `true` if `var` was added, else `false`

```c
Variable *scope_find_name(const Scope *scope, const char32_t *name)
```

> Returns pointer to variable with matching `name` if found, else `NULL`

```c
Scope *make_scope(void)
```

> Returns a new variable scope.

```c
void free_scope(Scope *scope)
```

> Frees a `scope` and all the variables inside of it.

