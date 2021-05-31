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
> \
> `next` stores the next function declaration.

```c
bool gen_stmt_func_decl(const AstNode *const node)
```

> Parse declaration (and potential definition) of function in `node`.
> \
> Return `true` if an error occurred.

```c
static FunctionDeclaration *create_function(const AstNodeFunctionProto *const func_proto, char *name, bool is_private, bool *err)
```

> Add new LLVM function named `name` from `func_proto`.
> \
> If `is_private` is true the function will be private (statically linked).
> \
> Else, the function will be globally available.
> \
> Set `err` if an error occurred.

```c
static LLVMTypeRef *parse_func_param(const AstNodeFunctionProto *const func_proto, FunctionDeclaration *const func, bool *err)
```

> Setup `func` params by parsing `func_proto`.
> \
> Set `err` if an error occurred.

```c
Expr gen_expr_function_call(const AstNodeExpr *const expr, Type type, bool *err)
```

> Builds a function call from `expr`.
> \
> Set `err` if an error occurred.

```c
static bool gen_function(const AstNode *const node, FunctionDeclaration *func)
```

> Create a native LLVM function.

```c
FunctionDeclaration *find_func_by_name(const char *name)
```

> Return function declaration called `name`, or `NULL` if not found.

