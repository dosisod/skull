# skull/semantic/scope

```c
typedef struct Scope {
	HashTable *symbols;

	Scope *next;
	Scope *last;
	Scope *child;
	Scope *parent;
}
```

> `Scope` stores all of the variables declared in a specific scope.
> \
> `symbols` is a hashtable of the symbols declared in the current scope.
> \
> `child` stores a nested scope with more variables in it.

```c
bool scope_add_var(SemanticState *state, Symbol *symbol)
```

> Add `symbol` (as variable) to current scope.
> \
> Returns `true` if `var` was added, else `false`

```c
Symbol *scope_find_name(const Scope *const scope, char *name)
```

> Returns pointer to variable with matching `name` if found, else `NULL`
> \
> This function might modify the contents of `name` durring execution, but will
> always keep the data the same after the function has finished.

```c
Scope *make_scope(void)
```

> Returns a new variable scope.

```c
Symbol *scope_find_var(SemanticState *state, const Token *const token)
```

> Try and find a variable stored in `token`.
> \
> Allow for finding of uninitialized variables by setting `allow_uninitialized`.
> \
> Return `NULL` if variable was not found.

```c
void make_child_scope(SemanticState *state)
```

> Add a child scope to the current and replace current scope with new one.

```c
void make_adjacent_scope(SemanticState *state)
```

> Make a scope adjacent to the current scope.
> \
> An adjacent scope is one that is on the same level as the current scope and
> after a child scope.

```c
static Scope *find_scope_head(SemanticState *state)
```

> Find the head of the current scope (without going to the parent node).

```c
void reset_scope_head(SemanticState *state)
```

> Move the current scope to the scope head.

```c
void restore_parent_scope(SemanticState *state)
```

> Free current scope, set current scope to parent scope.

```c
void free_scope(Scope *scope)
```

> Frees a `scope` and all the symbols inside of it.

