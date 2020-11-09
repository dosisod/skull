# skull/llvm/assign

```c
void llvm_make_var_def(AstNode **node)
```

> Builds a variable from `node`.

```c
void llvm_make_assign(AstNode **node)
```

> Build a LLVM `load` operation from `node`.

```c
void llvm_make_assign_(Variable *const var, const AstNode *const node)
```

> Internal function to build LLVM assignment from `node` to `var.

```c
LLVMValueRef llvm_assign_identifier(Variable *const var, const AstNode *const node)
```

> Return LLVM for to load an existing identifier `node` to `var`.

