# skull/llvm/shared

```c
typedef struct {
	LLVMBuilderRef builder;
	LLVMContextRef ctx;
	const char *filename;
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

