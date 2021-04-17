# skull/codegen/assign

```c
void gen_stmt_var_def(AstNode **node)
```

> Builds a variable from `node`.

```c
void gen_stmt_var_assign(AstNode **node)
```

> Build a LLVM `load` operation from `node`.

```c
Expr node_to_expr(Type type, const AstNode *const node, const Variable *const var)
```

> Create an expression from `node` with type `type`.
> \
> Optionally pass `var` if expression is going to be assigned to a variable.

```c
void assign_value_to_var(LLVMValueRef value, Variable *const var)
```

> Assign `value` to `var`.

```c
void create_type_alias(AstNode **node)
```

> Create a type alias from `node`.

