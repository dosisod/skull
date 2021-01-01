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
Expr node_to_expr(const Type *const type, const AstNode *const node, const Variable *const var)
```

> Create an expression from `node` with type `type`.
> \
> Optionally pass `var` if expression is going to be assigned to a variable.

```c
LLVMValueRef llvm_assign_identifier(const Type *const type, const AstNode *const node, const Variable *const var)
```

> Return LLVM for to load an existing identifier `node` with type `type`.
> \
> Optionally pass `var` if result is expected to be assigned to a variable.

