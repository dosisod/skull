# skull/codegen/assign

```c
bool gen_stmt_var_def(AstNode *node)
```

> Builds a variable definition from `node`.
> \
> Return `true` if an error occurred.

```c
bool gen_stmt_var_assign(AstNode *node)
```

> Assign a to a variable from `node`.
> \
> Return `true` if an error occurred.

```c
static void assign_value_to_var(LLVMValueRef value, Variable *const var)
```

> Assign `value` to `var`.

