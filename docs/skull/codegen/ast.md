# skull/codegen/ast

```c
bool codegen_str(char *const str_)
```

> Generate code from `str_`.
> \
> Return `true` if errors occurred.

```c
Expr gen_node(AstNode *node)
```

> Internal LLVM parser.
> \
> Return expr from an `AST_NODE_RETURN` if one was found.

```c
void assert_sane_child(AstNode *node)
```

> Verify that `node` doens't contain child node if it shouldn't.

```c
static Expr _gen_node(AstNode **node)
```

> Internal `gen_node` function.

```c
static void gen_expr_node(const AstNode *node)
```

> Generate a (function) expression from `node`.

