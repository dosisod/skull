# skull/codegen/ast

```c
bool gen_tree(AstNode *node)
```

> Run code generator for tree starting at `node`.

```c
Expr gen_node(AstNode *node, bool *err)
```

> Return expr from an `AST_NODE_RETURN` if one was found.

```c
static Expr _gen_node(AstNode **node, bool *err)
```

> Internal `gen_node` function.

```c
static bool gen_expr_node(const AstNode *node)
```

> Generate a (function) expression from `node`.
> \
> Return `true` if error occurred.

