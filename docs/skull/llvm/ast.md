# skull/llvm/ast.c

```c
void str_to_llvm_ir(char *const str_, LLVMValueRef func, LLVMModuleRef module, LLVMBuilderRef builder)
```

> Convert skull code from `str_` into LLVM IR (using `func` and `module`).

```c
void node_to_llvm_ir(AstNode *node)
```

> Internal LLVM IR parser.

