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

```c
static Variable *node_to_var(const AstNode *const node)
```

> Make and add a variable from `node` to Skull state.
> \
> Return `NULL` if an error occurred.

```c
static __attribute__((pure)) const AstNodeExpr *leftmost_expr(const AstNodeExpr *expr)
```

> Returns the left-most expr that is either a constant, variable, or function.

```c
static Type var_def_node_to_type(const AstNode *node)
```

> Return a variable type based on `node`, `NULL` if an error occurred.

