# skull/codegen/func

```c
typedef struct FunctionDeclaration {
	char *name;
	Location location;

	LLVMValueRef ref;
	LLVMTypeRef type;

	unsigned short num_params;
	Type *param_types;
	char32_t **param_names;

	Type return_type;

	_Bool was_called : 1;
	_Bool is_external : 1;
	_Bool is_export : 1;

	AstNodeFunctionParam **params;
}
```

> Stores a function declaration.
> \
> `name` is the name of the declared function.
> \
> `location` is the location of the function name.
> \
> `ref` is the LLVM function handle.
> \
> `type` is the LLVM function type.
> \
> `num_params` stores the number of params a function can take.
> \
> `param_types` is the Skull type for each function param.
> \
> `return_type` is the Skull type that the function returns.

```c
bool gen_stmt_func_decl(const AstNode *const node)
```

> Parse declaration (and potential definition) of function in `node`.
> \
> Return `true` if an error occurred.

```c
static void create_function(FunctionDeclaration *func)
```

> Add new LLVM function from `func`.
> \
> Else, the function will be globally available.

```c
static LLVMTypeRef *parse_func_param(const FunctionDeclaration *func)
```

> Setup function params of `func`.

```c
Expr gen_expr_function_call(const AstNodeExpr *const expr, Type type, bool *err)
```

> Builds a function call from `expr`.
> \
> Set `err` if an error occurred.

```c
static bool gen_function_def(const AstNode *const node, FunctionDeclaration *func)
```

> Create a native LLVM function.

```c
FunctionDeclaration *find_func_by_name(const char *name)
```

> Return function declaration called `name`, or `NULL` if not found.

