# skull/llvm/func.c

```c
void declare_external_function(AstNode *node)
```

> Store function name of externaly declared function in `node`.

```c
LLVMValueRef llvm_make_function(const AstNode *const node)
```

> Builds a function declaration from `node`.

