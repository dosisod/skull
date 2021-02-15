# skull/llvm/var

```c
Expr token_to_expr(const Token *const token, Variable **variable)
```

> Convert `token` to an expression.
> \
> If `variable` is and `token` is a variable, store the found variable there.

```c
Variable *node_to_var(const AstNode *const node)
```

> Make and add a variable from `node` to Skull state.

```c
Expr token_to_simple_expr_typed(const Type *const type, const Token *const token)
```

> Make an expression from `token`, checking for compatibility with `type`.

```c
Expr token_to_simple_expr(const Token *const token)
```

> Make a simple expression (const literal) from `token`.

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

