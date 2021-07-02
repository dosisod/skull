# skull/semantic/shared

```c
typedef struct {
	Scope *scope;
	HashTable *function_decls;
	HashTable *type_aliases;
}
```

> Stores semantic analyzer global state.

```c
void free_state_semantic(void)
```

> Free everything about a Skull compiler instance.

