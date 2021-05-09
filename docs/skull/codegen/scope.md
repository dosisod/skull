# skull/codegen/scope

```c
Variable *scope_find_var(const Token *const token, bool *err)
```

> Try and find a variable stored in `token`.
> \
> Set `err` if an error occurred.

```c
void make_sub_scope(Scope **old, Scope **new)
```

> Make new scope and set the current scope to be a sub-scope of the new one.

```c
void restore_sub_scope(Scope **old, Scope **new)
```

> Free the new scope, set the current scope to the old sub-scope.

