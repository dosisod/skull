# skull/llvm/ast.c

```c
void str_to_llvm_ir(char *str_, LLVMValueRef func, LLVMBuilderRef builder, LLVMContextRef ctx, LLVMModuleRef mod)
```

> Convert skull code from `str` into LLVM IR (using `builder`, `ctx`,  and `scope`).

```c
void str_to_llvm_ir_(AstNode *node, LLVMValueRef func, LLVMBuilderRef builder, LLVMContextRef ctx, LLVMModuleRef mod)
```

> Internal LLVM IR parser.

