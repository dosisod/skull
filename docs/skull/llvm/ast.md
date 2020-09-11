# skull/llvm/ast.c

```c
void str_to_llvm_ir(char *str_, LLVMValueRef func, LLVMBuilderRef builder, LLVMContextRef llvm_ctx)
```

> Convert skull code from `str` into LLVM IR (using `builder` and `ctx).

