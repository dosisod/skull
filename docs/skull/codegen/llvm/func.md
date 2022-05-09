# skull/codegen/llvm/func

```c
void gen_stmt_func_decl(SemanticState *semantic_state, const AstNode *const node, SkullStateLLVM *state)
```

> Parse declaration (and potential definition) of function in `node`.

```c
static void add_func(Symbol *symbol, const SkullStateLLVM *state)
```

> Add new LLVM function from `symbol`.

```c
static LLVMTypeRef *parse_func_param(const FunctionDeclaration *func, const SkullStateLLVM *state)
```

> Setup function params of `func`.

```c
Expr gen_expr_func_call(const AstNodeFunctionCall *const func_call, const SkullStateLLVM *state)
```

> Builds a function call from `func_call`.

```c
static void gen_function_def(SemanticState *semantic_state, const AstNode *const node, Symbol *symbol, SkullStateLLVM *state)
```

> Create a native LLVM function.

