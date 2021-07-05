# skull/semantic/shared

```c
typedef struct {
	Scope *scope;
}
```

> Stores semantic analyzer global state.

```c
void free_state_semantic(void)
```

> Free everything about a Skull compiler instance.

