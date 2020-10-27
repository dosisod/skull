# skull/llvm/ast.c

```c
void str_to_llvm_ir(char *const str_, LLVMValueRef func_, LLVMModuleRef module_, LLVMBuilderRef _builder)
```

> Convert skull code from `str_` into LLVM IR (using `func_` and `module_`).

```c
void node_to_llvm_ir(AstNode *node)
```

> Internal LLVM IR parser.

