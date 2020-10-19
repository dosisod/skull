# skull/llvm/ast.c

```c
void str_to_llvm_ir(char *const str_, LLVMValueRef func_, LLVMModuleRef module_)
```

> Convert skull code from `str_` into LLVM IR (using `func_` and `module_`).

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
void llvm_make_math_oper(Variable *var, const AstNode *node, MathOper oper, const char32_t *panic)
```

> Build LLVM for assigning math operation `oper` from `node` to `var`.

```c
LLVMValueRef llvm_make_add(Variable *var, const Token *lhs, const Token *rhs)
```

> Build LLVM for assining addition of `lhs` and `rhs` to `var`.

```c
LLVMValueRef llvm_make_sub(Variable *var, const Token *lhs, const Token *rhs)
```

> Build LLVM for assining subtraction of `lhs` and `rhs` to `var`.

```c
LLVMValueRef llvm_make_mult(Variable *var, const Token *lhs, const Token *rhs)
```

> Build LLVM for assining multiplication of `lhs` and `rhs` to `var`.

```c
LLVMValueRef llvm_make_div(Variable *var, const Token *lhs, const Token *rhs)
```

> Build LLVM for assining division of `lhs` and `rhs` to `var`.

```c
void declare_external_function(AstNode *node)
```

> Store function name of externaly declared function in `node`.

```c
void llvm_make_function(AstNode *node)
```

> Builds a function declaration from `node`.

```c
void llvm_make_assign(AstNode **node)
```

> Build a LLVM `load` operation from `node`.

```c
void llvm_make_assign_(Variable *const var, const AstNode *const node)
```

> Internal function to build LLVM assignment from `node` to `var.

```c
void llvm_assign_identifier(Variable *const var, const AstNode *const node)
```

> Build LLVM to assign an existing identifier `node` to `var`.

