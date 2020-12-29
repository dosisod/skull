# skull/llvm/assign

```c
void llvm_make_var_def(AstNode **node)
```

> Builds a variable from `node`.

```c
void llvm_make_var_assign(AstNode **node)
```

> Build a LLVM `load` operation from `node`.

```c
LLVMValueRef llvm_get_value_for_var(const Variable *const var, const AstNode *const node)
```

> Based on `var` and `node`, try to make an LLVM value that is assignable to `var.

```c
LLVMValueRef llvm_assign_identifier(const Variable *const var, const AstNode *const node)
```

> Return LLVM for to load an existing identifier `node` to `var`.

