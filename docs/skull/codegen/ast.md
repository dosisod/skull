# skull/codegen/ast

```c
void codegen_str(char *const str_)
```

> Generate code from `str_`.

```c
bool gen_node(AstNode *node)
```

> Internal LLVM parser.
> \
> Return true if there was a `AST_NODE_RETURN` node was parsed, else false.

