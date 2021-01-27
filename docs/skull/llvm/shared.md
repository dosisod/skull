# skull/llvm/shared

```c
typedef struct {
	LLVMBuilderRef builder;
	LLVMContextRef ctx;
	LLVMModuleRef module;
	LLVMValueRef current_func;
	LLVMValueRef main_func;
	Scope *scope;
	HashTable *function_decls;
	HashTable *type_aliases;
}
```

> Shared state for a given Skull program.

