# skull/llvm/oper

```c
LLVMValueRef llvm_make_add(const Type *type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Build LLVM for assining addition of `lhs` and `rhs`.

```c
LLVMValueRef llvm_make_sub(const Type *type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Build LLVM for assining subtraction of `lhs` and `rhs`.

```c
LLVMValueRef llvm_make_mult(const Type *type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Build LLVM for assining multiplication of `lhs` and `rhs`.

```c
LLVMValueRef llvm_make_div(const Type *type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Build LLVM for assining division of `lhs` and `rhs`.

```c
LLVMValueRef llvm_make_oper(const Type *type, const AstNode *node, Operation *oper)
```

> Return LLVM for assigning operation `oper` from `node`.

```c
LLVMValueRef llvm_token_to_val(const Type *type, const Token *token)
```

> Return LLVM equivalent of `token`, checking for compatibility with `type`.

