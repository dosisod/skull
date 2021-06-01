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
static bool _gen_stmt_var_assign(Variable *var, AstNode **node)
```

> Does the actual assignment of `node` to `var`.
> \
> Return `true` if an error occurred.

```c
static void assign_value_to_var(LLVMValueRef value, Variable *const var)
```

> Assign `value` to `var`.

```c
bool create_type_alias(AstNode **node)
```

> Create a type alias from `node`.
> \
> Return `true` if an error occurred.

```c
static Variable *node_to_var(const AstNode *const node, bool *err)
```

> Make and add a variable from `node` to Skull state.

```c
static __attribute__((pure)) const AstNodeExpr *leftmost_expr(const AstNodeExpr *expr)
```

> Returns the left-most expr that is either a constant, variable, or function.

```c
static Type var_def_node_to_type(const AstNode *node, bool *err)
```

> Return a variable type based on `node`.

