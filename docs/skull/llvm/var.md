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

```c
#define SCOPE_FIND_VAR(var, token, var_name) \
	char32_t *const (var_name) = token_str((token)); \
	Variable *const (var) = scope_find_name(SCOPE, (var_name)); \
	if (!(var)) { \
		PANIC(ERR_VAR_NOT_FOUND, { .str = (var_name) }); \
	}
```

> Find the variable `var` stored in `token`.
> \
> Variable name is stored in `var_name`, and must be freed.

