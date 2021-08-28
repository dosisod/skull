# skull/codegen/assign

```c
void gen_stmt_var_def(AstNodeVarDef *var_def)
```

> Builds a variable definition from `expr_node`.

```c
void gen_stmt_var_assign(AstNodeVarAssign *var_assign)
```

> Assign a to a variable from `var_assign`.

```c
static void assign_value_to_var(LLVMValueRef value, Variable *const var)
```

> Assign `value` to `var`.

