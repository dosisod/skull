# skull/compiler/scope

```c
typedef struct Scope {
	HashTable *vars;

	Scope *sub_scope;
}
```

> `Scope` stores all of the variables declared in a specific scope.
> \
> `vars_used` stores how many variables are in a given scope.
> \
> `vars` is an array of all of the variables in a scope.
> \
> `sub_scope` stores a nested scope with more variables in it.

```c
bool scope_add_var(Scope **scope, Variable *const var)
```

> Add variable `var` to `scope`.
> \
> Returns `true` if `var` was added, else `false`

```c
Variable *scope_find_name(const Scope *const scope, const char *name)
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

