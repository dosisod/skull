# skull/llvm/assign

```c
void llvm_make_type_alias(AstNode **node)
```

> Create a type alias from `node`.

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
Expr llvm_make_bool_expr(const AstNode *const node)
```

> Build LLVM code to handle boolean expressions from `node`.

```c
void llvm_assign_value_to_var(Variable *const var, LLVMValueRef value)
```

> Assign `value` to `var`.

```c
Expr llvm_assign_identifier(const Type *const type, const AstNode *const node, const Variable *const var)
```

> Return expression for assigning existing identifier `node` with type `type`.
> \
> Optionally pass `var` if result is expected to be assigned to a variable.
> \
> If `type` is not set, the expression type will not be checked.

