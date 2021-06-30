# skull/codegen/shared

```c
typedef struct {
	LLVMBuilderRef builder;
	LLVMContextRef ctx;
	LLVMModuleRef module;
	FunctionDeclaration *current_func;
	FunctionDeclaration *main_func;
	Scope *scope;
	HashTable *function_decls;
	HashTable *type_aliases;
	HashTable *template_types;
}
```

> Shared state for a given Skull program.

```c
static char *create_main_func_name(const char *filename)
```

> Convert/mangle `filename` into suitable name for "main" method for module.

```c
void free_state(void)
```

> Free everything about a Skull compiler instance.

