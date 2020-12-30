# skull/llvm/var

```c
void node_make_var(const AstNode *const node)
```

> Make and add a variable from `node` to global scope.

```c
Expr token_to_simple_expr_typed(const Type *const type, const Token *const token)
```

> Make an expression from `token`, checking for compatibility with `type`.

```c
Expr token_to_simple_expr(const Token *const token)
```

> Make a simple expression (const literal) from `token`.

