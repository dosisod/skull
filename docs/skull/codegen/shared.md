# skull/codegen/shared

```c
typedef struct {
	Scope *scope;
	HashTable *function_decls;
	HashTable *type_aliases;
}
```

> Shared state for a given Skull program.

```c
void free_state(void)
```

> Free everything about a Skull compiler instance.

