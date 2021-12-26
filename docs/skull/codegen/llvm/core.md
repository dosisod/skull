# skull/codegen/llvm/core

```c
void gen_module(const AstNode *node)
```

> Run code generator for module starting at `node`.

```c
Expr gen_tree(const AstNode *node)
```

> Generate tree starting at `node`, returning an expr if one is returned.

