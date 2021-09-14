# skull/semantic/shared

```c
typedef struct {
	Scope *scope;
	FunctionDeclaration *main_func;
	FunctionDeclaration *current_func;
	FunctionDeclaration *last_func;
}
```

> Stores semantic analyzer global state.

```c
_Bool free_semantic_state(void)
```

> Free everything about a Skull compiler instance.
> \
> Return `true` if an error occurred.

