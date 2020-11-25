# skull/llvm/flow

```c
void llvm_make_return(AstNode *node)
```

> Builds an return statement from `node`.

```c
void llvm_make_while(AstNode *node)
```

> Builds LLVM for a while loop from `node`.

```c
void llvm_make_if(AstNode **node)
```

> Builds an if block from `node`.

```c
LLVMValueRef llvm_make_cond(AstNode *node)
```

> Try and parse a condition (something returning a bool) from `node`.

```c
void llvm_make_code_block(const char32_t *name, AstNode *node, LLVMBasicBlockRef block)
```

> Parse `node` while in a new scope. Branch to `block` when done.
> \
> `name` is the type of block: if, else, while, etc.

