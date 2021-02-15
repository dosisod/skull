# skull/llvm/ast

```c
void str_to_llvm(char *const str_)
```

> Generate LLVM from `str_`.

```c
bool node_to_llvm(AstNode *node)
```

> Internal LLVM parser.
> \
> Return true if there was a `AST_NODE_RETURN` node was parsed, else false.

