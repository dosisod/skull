# skull/codegen/var

```c
Variable *node_to_var(const AstNode *const node)
```

> Make and add a variable from `node` to Skull state.

```c
const Type *var_def_node_to_type(const AstNode *node)
```

> Return a variable type based on `node`.

```c
typedef struct {
	LLVMValueRef llvm_value;
	const Type *type;
}
```

> Expr stores all the data needed to do type comparisons with LLVMValueRef's.
> \
> `llvm_value` is the LLVM component.
> \
> `type` is the Skull usable which `llvm_value` evaluates to.
