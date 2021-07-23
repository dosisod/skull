# skull/codegen/assign

```c
bool gen_stmt_var_def(AstNode *expr_node)
```

> Builds a variable definition from `expr_node`.
> \
> Return `true` if an error occurred.

```c
bool gen_stmt_var_assign(AstNode *expr_node)
```

> Assign a to a variable from `expr_node`.
> \
> Return `true` if an error occurred.

```c
static void assign_value_to_var(LLVMValueRef value, Variable *const var)
```

> Assign `value` to `var`.

