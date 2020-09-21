# skull/llvm/ast.c

```c
void str_to_llvm_ir(char *str_, LLVMValueRef func_, LLVMBuilderRef builder_, LLVMContextRef ctx_, LLVMModuleRef module_)
```

> Convert skull code from `str` into LLVM IR (using `builder` and `ctx`).

```c
void node_to_llvm_ir(AstNode *node)
```

> Internal LLVM IR parser.

```c
void llvm_make_return(AstNode *node)
```

> Builds an return statement from `node`.

```c
void llvm_make_var_def(AstNode **node)
```

> Builds a variable from `node`.

```c
void llvm_make_if(AstNode *node)
```

> Builds an if block from `node`.

```c
void llvm_make_function(AstNode *node)
```

> Builds a function declaration from `node`.

