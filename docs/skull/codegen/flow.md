# skull/codegen/flow

```c
Expr gen_stmt_unreachable(void)
```

> Build an unreachable statement.

```c
Expr gen_stmt_return(AstNode **node, bool *err)
```

> Builds an return statement from `node`.
> \
> Set `err` if error occurrs.

```c
bool gen_control_while(AstNode **node)
```

> Builds LLVM for a while loop from `node`.
> \
> Return `true` if an error occurred.

```c
bool gen_control_if(AstNode **node)
```

> Builds an if block from `node`.

```c
static bool gen_control_if_(AstNode **node, LLVMBasicBlockRef entry, LLVMBasicBlockRef end)
```

> Internal function for building an `if` node.
> \
> Return `true` if an error occurred.

```c
static LLVMValueRef node_to_bool(const AstNode *const node, bool *err)
```

> Try and parse a condition (something returning a bool) from `node`.
> \
> Set `err` if an error occurred.

```c
static bool gen_control_code_block(const char *name, const AstNode *const node, LLVMBasicBlockRef block)
```

> Parse `node` while in a new scope. Branch to `block` if no return occurred.
> \
> `name` is the type of block: if, else, while, etc.
> \
> Return `true` if error occurred.

