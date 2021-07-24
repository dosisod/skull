# skull/codegen/llvm/shared

```c
char *create_main_func_name(const char *filename)
```

> Convert/mangle `filename` into suitable name for "main" method for module.

```c
void free_llvm_state(void)
```

> Free everything about a Skull LLVM compiler instance.

