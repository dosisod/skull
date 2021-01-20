# skull/llvm/oper

```c
Expr llvm_make_math_oper(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs, LLVMBuildX int_func, LLVMBuildX float_func)
```

> Returns the result of a mathematical operation on `lhs` and `rhs`.
> \
> Depending on whether `type` is an int or float, combine `lhs` and `rhs` using `int_func` or `float_func`.

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
Expr llvm_make_is_not(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for result of is not operator for `lhs` and `rhs`.

```c
Expr llvm_make_relational_oper(const Type *type, LLVMValueRef lhs, LLVMValueRef rhs, LLVMIntPredicate int_pred, LLVMRealPredicate float_pred)
```

> Return result of relational comparison on `lhs` and `rhs`.
> \
> Depending on whether `type` is an int or float, compare `lhs` and `rhs` using `int_func` or `float_func`.

```c
Expr llvm_make_less_than(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for result of less than operator for `lhs` and `rhs`.

```c
Expr llvm_make_gtr_than(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for result of greater than operator for `lhs` and `rhs`.

```c
Expr llvm_make_less_than_eq(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for result of less than or equal to operator for `lhs` and `rhs`.

```c
Expr llvm_make_gtr_than_eq(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for result of greater than or equal to operator for `lhs` and `rhs`.

```c
Expr llvm_make_logical_oper(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs, LLVMBuildX func)
```

> Return result of logical operation `func` on `lhs` and `rhs`.

```c
Expr llvm_make_and(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return result of logical "and" operation of `lhs` and `rhs`.

```c
Expr llvm_make_or(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return result of logical "or" operation of `lhs` and `rhs`.

```c
Expr llvm_make_xor(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return result of logical "xor" operation of `lhs` and `rhs`.

```c
Expr llvm_make_oper(const Type *const type, const AstNode *const node, Operation *oper)
```

> Return expression for operation `oper` from `node`.

