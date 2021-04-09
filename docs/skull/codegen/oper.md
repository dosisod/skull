# skull/codegen/oper

```c
Expr gen_expr_math_oper(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs, LLVMBuildX int_func, LLVMBuildX float_func)
```

> Returns the result of a mathematical operation on `lhs` and `rhs`.
> \
> Depending on whether `type` is an int or float, combine `lhs` and `rhs`
> using `int_func` or `float_func`.

```c
Expr gen_expr_add(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for addition of `lhs` and `rhs`.

```c
Expr gen_expr_sub(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for subtraction of `lhs` and `rhs`.

```c
Expr gen_expr_mult(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for multiplication of `lhs` and `rhs`.

```c
Expr gen_expr_div(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for division of `lhs` and `rhs`.

```c
Expr gen_expr_mod(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for modulus of `lhs` and `rhs`.

```c
Expr gen_expr_lshift(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for left shift of `lhs` and `rhs`.

```c
Expr gen_expr_rshift(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for logical right shift of `lhs` and `rhs`.

```c
Expr gen_expr_pow(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for taking `lhs` to the power of `rhs`.

```c
Expr gen_expr_not(const Type *type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for result of not operator for `rhs`.

```c
Expr gen_expr_unary_neg(const Type *type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for result of unary negation operator for `rhs`.

```c
Expr gen_expr_is(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for result of is operator for `lhs` and `rhs`.

```c
Expr gen_expr_is_str(LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for string-is operator against `lhs` and `rhs`.

```c
Expr create_and_call_builtin_oper(const Type *rtype, LLVMTypeRef type, const char *name, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Create a function called `name` (if it does not exist) which returns type
> `rtype`, and has operands of type `type`. Afterwards, call the new function
> with the `lhs` and `rhs` operands.

```c
Expr gen_expr_is_not(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for result of is not operator for `lhs` and `rhs`.

```c
Expr gen_expr_relational_oper(const Type *type, LLVMValueRef lhs, LLVMValueRef rhs, LLVMIntPredicate int_pred, LLVMRealPredicate float_pred)
```

> Return result of relational comparison on `lhs` and `rhs`.
> \
> Depending on whether `type` is an int or float, compare `lhs` and `rhs` using
> `int_func` or `float_func`.

```c
Expr gen_expr_less_than(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for result of less than operator for `lhs` and `rhs`.

```c
Expr gen_expr_gtr_than(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for result of greater than operator for `lhs` and `rhs`.

```c
Expr gen_expr_less_than_eq(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for result of less than or equal to operator for `lhs` and
> `rhs`.

```c
Expr gen_expr_gtr_than_eq(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for result of greater than or equal to operator for `lhs`
> and `rhs`.

```c
Expr gen_expr_logical_oper(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs, LLVMBuildX func)
```

> Return result of logical operation `func` on `lhs` and `rhs`.

```c
Expr gen_expr_and(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return result of logical "and" operation of `lhs` and `rhs`.

```c
Expr gen_expr_or(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return result of logical "or" operation of `lhs` and `rhs`.

```c
Expr gen_expr_xor(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return result of logical "xor" operation of `lhs` and `rhs`.

```c
Expr gen_expr_oper(const Type *const type, const AstNodeExpr *const expr, Operation *oper)
```

> Return expression for operation `oper` on `expr`.

```c
Expr token_to_expr(const Token *const token, Variable **variable)
```

> Convert `token` to an expression.
> \
> If `variable` is and `token` is a variable, store the found variable there.

```c
Expr token_to_simple_expr_typed(const Type *const type, const Token *const token)
```

> Make an expression from `token`, checking for compatibility with `type`.

```c
Expr token_to_simple_expr(const Token *const token)
```

> Make a simple expression (const literal) from `token`.
