# skull/llvm/ast.c

```c
void str_to_llvm_ir(char *const str_, LLVMValueRef func_, LLVMBuilderRef builder_, LLVMModuleRef module_)
```

> Convert skull code from `str_` into LLVM IR (using `func_`, `builder_`, and `module_`).

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

```c
void llvm_make_assign(AstNode **node)
```

> Build a LLVM `load` operation from `node`.

