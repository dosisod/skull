# skull/llvm/var

```c
void node_make_var(const AstNode *const node)
```

> Make and add a variable from `node` to global scope.

```c
LLVMValueRef llvm_parse_var(const Variable *const var, const Token *const token)
```

> Make an `LLVMValueRef` for a given `var` from `token`.

```c
LLVMValueRef llvm_parse_token(const Token *const token)
```

> Make an `LLVMValueRef` from `token`.

