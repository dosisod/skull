# skull/codegen/ast

```c
void gen_module(AstNode *node)
```

> Run code generator for module starting at `node`.

```c
Expr gen_tree(AstNode *node)
```

> Generate tree starting at `node`, returning an expr if one is returned.

```c
static Expr _gen_tree(AstNode **node)
```

> Internal `gen_tree` function.

