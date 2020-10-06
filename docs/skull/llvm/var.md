# skull/llvm/var.c

```c
void var_to_llvm_ir(Variable *var, LLVMBuilderRef builder, LLVMContextRef ctx)
```

> Convert a Skull variable `var` into the LLVM IR equivalent.

```c
const char32_t *node_make_var(const AstNode *const node, Scope *const scope)
```

> Make and add a variable from `node` to `scope`.
> \
> Returns pointer to error message if one occurs, else `NULL`.

