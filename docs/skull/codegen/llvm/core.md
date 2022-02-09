# skull/codegen/llvm/core

```c
void gen_module(const AstNode *node, SkullStateLLVM *state)
```

> Run code generator for module starting at `node`.

```c
Expr gen_tree(const AstNode *node, SkullStateLLVM *state)
```

> Generate tree starting at `node`, returning an expr if one is returned.

