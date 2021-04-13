# skull/codegen/var

```c
Variable *node_to_var(const AstNode *const node)
```

> Make and add a variable from `node` to Skull state.

```c
const AstNodeExpr *leftmost_expr(const AstNodeExpr *expr)
```

> Returns the left-most expr that is either a constant, variable, or function.

```c
const Type *var_def_node_to_type(const AstNode *node)
```

> Return a variable type based on `node`.

```c
typedef struct {
	LLVMValueRef value;
	const Type *type;
}
```

> Expr stores all the data needed to do type comparisons with LLVMValueRef's.
> \
> `value` is the LLVM component.
> \
> `type` is the Skull usable which `value` evaluates to.

