# skull/llvm/func

```c
typedef struct FunctionDeclaration {
	char *name;
	LLVMValueRef function;
	LLVMTypeRef type;

	unsigned num_params;
	const Type **param_types;
	char32_t **param_names;

	const Type *return_type;

	FunctionDeclaration *next;
}
```

> Stores a function declaration.
> \
> `name` is the name of the declared function.
> \
> `function` is the LLVM function handle.
> \
> `type` is the LLVM function type.
> \
> `num_params` stores the number of params a function can take.
> \
> `param_types` is the Skull type for each function param.
> \
> `return_type` is the Skull type that the function returns.
> \
> `next` stores the next function declaration.

```c
void declare_function(const AstNode *const node)
```

> Parse declaration (and potential definition) of function in `node`.

```c
Expr llvm_make_function_call(const AstNode *const node)
```

> Builds a function call from `node`.

```c
void define_function(const AstNode *const node, FunctionDeclaration *func)
```

> Create a native LLVM function.

