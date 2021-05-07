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

