# skull/llvm/shared

```c
typedef struct {
	LLVMBuilderRef builder;
	LLVMContextRef ctx;
	LLVMModuleRef module;
	LLVMValueRef current_func;
	LLVMValueRef main_func;
	Scope *scope;
}
```

> Shared state for a given Skull program.

