# skull/llvm/oper

```c
Expr llvm_make_add(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for addition of `lhs` and `rhs`.

```c
Expr llvm_make_sub(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for subtraction of `lhs` and `rhs`.

```c
Expr llvm_make_mult(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for multiplication of `lhs` and `rhs`.

```c
Expr llvm_make_div(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for division of `lhs` and `rhs`.

```c
Expr llvm_make_is(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for result of is operator for `lhs` and `rhs`.

```c
Expr llvm_make_less_than(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for result of less than operator for `lhs` and `rhs`.

```c
Expr llvm_make_gtr_than(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for result of greater than operator for `lhs` and `rhs`.

```c
Expr llvm_make_oper(const Type *const type, const AstNode *const node, Operation *oper)
```

> Return expression for operation `oper` from `node`.

