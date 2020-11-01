# skull/llvm/math.c

```c
LLVMValueRef llvm_make_add(Variable *var, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Build LLVM for assining addition of `lhs` and `rhs` to `var`.

```c
LLVMValueRef llvm_make_sub(Variable *var, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Build LLVM for assining subtraction of `lhs` and `rhs` to `var`.

```c
LLVMValueRef llvm_make_mult(Variable *var, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Build LLVM for assining multiplication of `lhs` and `rhs` to `var`.

```c
LLVMValueRef llvm_make_div(Variable *var, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Build LLVM for assining division of `lhs` and `rhs` to `var`.

```c
void llvm_make_math_oper(Variable *var, const AstNode *node, MathOper *oper, const char *panic)
```

> Build LLVM for assigning math operation `oper` from `node` to `var`.

