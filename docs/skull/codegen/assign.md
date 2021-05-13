# skull/codegen/assign

```c
bool gen_stmt_var_def(AstNode **node)
```

> Builds a variable definition from `node`.
> \
> Return `true` if an error occurred.

```c
bool gen_stmt_var_assign(AstNode **node)
```

> Assign a to a variable from `node`.
> \
> Return `true` if an error occurred.

```c
bool _gen_stmt_var_assign(Variable *var, AstNode **node)
```

> Does the actual assignment of `node` to `var`.
> \
> Return `true` if an error occurred.

```c
Expr node_to_expr(Type type, const AstNode *const node, bool *err)
```

> Create an expression from `node` with type `type`.
> \
> Optionally pass `var` if expression is going to be assigned to a variable.
> \
> Set `err` if an error occurred.

```c
void assign_value_to_var(LLVMValueRef value, Variable *const var)
```

> Assign `value` to `var`.

```c
bool create_type_alias(AstNode **node)
```

> Create a type alias from `node`.
> \
> Return `true` if an error occurred.

