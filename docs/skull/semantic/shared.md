# skull/semantic/shared

```c
typedef struct {
	Scope *scope;
	FunctionDeclaration *main_func;
	FunctionDeclaration *current_func;
	FunctionDeclaration *last_func;
	unsigned while_loop_depth;
	HashTable *dynamic_pointers;
}
```

> Stores semantic analyzer global state.

```c
void free_semantic_state(void)
```

> Free everything about a Skull semantic instance.

