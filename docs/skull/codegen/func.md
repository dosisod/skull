# skull/codegen/func

```c
void gen_stmt_func_decl(const AstNode *const node)
```

> Parse declaration (and potential definition) of function in `node`.

```c
static void add_func(FunctionDeclaration *func)
```

> Add new LLVM function from `func`.

```c
static LLVMTypeRef *parse_func_param(const FunctionDeclaration *func)
```

> Setup function params of `func`.

```c
Expr gen_expr_func_call(const AstNodeFunctionCall *const func_call)
```

> Builds a function call from `func_call`.

```c
static void gen_function_def(const AstNode *const node, FunctionDeclaration *func)
```

> Create a native LLVM function.

