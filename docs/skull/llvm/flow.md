# skull/llvm/flow

```c
void llvm_make_return(const AstNode *const node)
```

> Builds an return statement from `node`.

```c
void llvm_make_while(const AstNode *const node)
```

> Builds LLVM for a while loop from `node`.

```c
void llvm_make_if(AstNode **node)
```

> Builds an if block from `node`.

```c
void llvm_make_if_(AstNode **node, LLVMBasicBlockRef entry, LLVMBasicBlockRef end)
```

> Internal function for building an `if` node.

```c
LLVMValueRef llvm_make_cond(const AstNode *const node)
```

> Try and parse a condition (something returning a bool) from `node`.

```c
LLVMValueRef llvm_get_bool_from_token(const Token *token)
```

> Returns an LLVM value parsed from `token`.

```c
void llvm_make_code_block(const char32_t *name, const AstNode *const node, LLVMBasicBlockRef block)
```

> Parse `node` while in a new scope. Branch to `block` when done.
> \
> `name` is the type of block: if, else, while, etc.

