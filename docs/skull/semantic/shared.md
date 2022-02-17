# skull/semantic/shared

```c
typedef struct {
	Scope *scope;
	Symbol *main_func;
	Symbol *current_func;
	Symbol *last_func;
	unsigned while_loop_depth;
	HashTable *dynamic_refs;
}
```

> Stores semantic analyzer global state.

```c
void free_semantic_state(void)
```

> Free everything about a Skull semantic instance.

