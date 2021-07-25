# skull/codegen/assign

```c
bool gen_stmt_var_def(AstNodeVarDef *var_def)
```

> Builds a variable definition from `expr_node`.
> \
> Return `true` if an error occurred.

```c
bool gen_stmt_var_assign(AstNodeVarAssign *var_assign)
```

> Assign a to a variable from `var_assign`.
> \
> Return `true` if an error occurred.

```c
static void assign_value_to_var(LLVMValueRef value, Variable *const var)
```

> Assign `value` to `var`.

