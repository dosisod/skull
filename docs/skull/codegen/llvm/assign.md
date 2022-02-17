# skull/codegen/llvm/assign

```c
void gen_stmt_var_def(const AstNodeVarDef *var_def, SkullStateLLVM *state)
```

> Builds a variable definition from `var_def`.

```c
void gen_stmt_var_assign(const AstNodeVarAssign *var_assign, SkullStateLLVM *state)
```

> Assign a to a variable from `var_assign`.

```c
static void assign_value_to_var(LLVMValueRef value, Symbol *symbol, const SkullStateLLVM *state)
```

> Assign `value` to `symbol`.

