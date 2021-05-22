# skull/codegen/var

```c
Variable *node_to_var(const AstNode *const node, bool *err)
```

> Make and add a variable from `node` to Skull state.

```c
__attribute__((pure)) const AstNodeExpr *leftmost_expr(const AstNodeExpr *expr)
```

> Returns the left-most expr that is either a constant, variable, or function.

```c
static Type var_def_node_to_type(const AstNode *node, bool *err)
```

> Return a variable type based on `node`.

