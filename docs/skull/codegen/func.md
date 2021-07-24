# skull/codegen/func

```c
bool gen_stmt_func_decl(const AstNode *const node)
```

> Parse declaration (and potential definition) of function in `node`.
> \
> Return `true` if an error occurred.

```c
static void add_func(FunctionDeclaration *func)
```

> Add new LLVM function from `func`.

```c
static LLVMTypeRef *parse_func_param(const FunctionDeclaration *func)
```

> Setup function params of `func`.

```c
Expr gen_expr_func_call(const AstNodeFunctionCall *const func_call, Type type, bool *err)
```

> Builds a function call from `expr`.
> \
> Set `err` if an error occurred.

```c
static bool gen_function_def(const AstNode *const node, FunctionDeclaration *func)
```

> Create a native LLVM function.

