# skull/llvm/var.c

```c
void var_to_llvm_ir(Variable *var, LLVMBuilderRef builder, LLVMContextRef ctx)
```

> Convert a Skull variable `var` into the LLVM IR equivalent.

```c
const char32_t *node_make_var(const AstNode *node, Scope *scope, bool is_const)
```

> Make and add a variable from `node` to `scope`.
> \
> Added variable will be constant if `is_const` is true.
> \
> Returns pointer to error message if one occurs, else `NULL`.

