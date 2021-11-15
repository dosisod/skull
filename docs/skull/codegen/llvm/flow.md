# skull/codegen/llvm/flow

```c
Expr gen_stmt_unreachable(void)
```

> Build an unreachable statement.

```c
Expr gen_stmt_return(const AstNode *node)
```

> Builds an return statement from `node`.

```c
void gen_control_while(const AstNode *node)
```

> Builds LLVM for a while loop from `node`.

```c
void gen_control_if(const AstNode **node)
```

> Builds an if block from `node`.

```c
static void gen_control_if_(const AstNode **node, LLVMBasicBlockRef entry, LLVMBasicBlockRef end)
```

> Internal function for building an `if` node.

```c
static void gen_control_code_block(const AstNode *const node, LLVMBasicBlockRef block)
```

> Parse `node` while in a new scope. Branch to `block` if no return occurred.

