# skull/codegen/scope

```c
Variable *scope_find_var(const Token *const token)
```

> Try and find a variable stored in `token`.
> \
> Return `NULL` if variable was not found.

```c
void make_sub_scope(Scope **old, Scope **new)
```

> Make new scope and set the current scope to be a sub-scope of the new one.

```c
void restore_sub_scope(Scope **old, Scope **new)
```

> Free the new scope, set the current scope to the old sub-scope.

