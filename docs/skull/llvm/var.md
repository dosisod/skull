# skull/llvm/var.c

```c
void node_make_var(const AstNode *const node, Scope *const scope)
```

> Make and add a variable from `node` to `scope`.

```c
LLVMValueRef llvm_parse_var(const Variable *const var, const Token *const token)
```

> Make an `LLVMValueRef` for a given `var` from `token`.

