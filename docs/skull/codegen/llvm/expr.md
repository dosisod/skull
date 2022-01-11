# skull/codegen/llvm/expr

```c
typedef struct {
	LLVMValueRef value;
	const Type *type;
}
```

> Expr stores all the data needed to do type comparisons with LLVMValueRef's.
> \
> `value` is the LLVM component.
> \
> `type` is the Skull usable which `value` evaluates to.

```c
Expr gen_expr(const AstNodeExpr *const expr)
```

> Return expression for `expr`.

```c
static Expr gen_expr_identifier(const AstNodeExpr *expr)
```

> Return expression for identifier `token` with type `type`.

```c
static Expr gen_expr_const(const AstNodeExpr *expr)
```

> Make a simple expression (const literal) from `expr`.

```c
static Expr gen_expr_math_oper(const Expr *lhs, LLVMValueRef rhs, LLVMBuildX int_func, LLVMBuildX float_func)
```

> Returns the result of a mathematical operation on `lhs` and `rhs`.
> \
> Depending on whether `type` is an int or float, combine `lhs` and `rhs`
> using `int_func` or `float_func`.

```c
static Expr gen_expr_add(const Expr *lhs, LLVMValueRef rhs)
```

> Return expression for addition of `lhs` and `rhs`.

```c
static Expr gen_expr_sub(const Expr *lhs, LLVMValueRef rhs)
```

> Return expression for subtraction of `lhs` and `rhs`.

```c
static Expr gen_expr_mult(const Expr *lhs, LLVMValueRef rhs)
```

> Return expression for multiplication of `lhs` and `rhs`.

```c
static Expr gen_expr_div(const Expr *lhs, LLVMValueRef rhs)
```

> Return expression for division of `lhs` and `rhs`.

```c
static Expr gen_expr_mod(const Expr *lhs, LLVMValueRef rhs)
```

> Return expression for modulus of `lhs` and `rhs`.

```c
static Expr gen_expr_lshift(const Expr *lhs, LLVMValueRef rhs)
```

> Return expression for left shift of `lhs` and `rhs`.

```c
static Expr gen_expr_rshift(const Expr *lhs, LLVMValueRef rhs)
```

> Return expression for logical right shift of `lhs` and `rhs`.

```c
static Expr gen_expr_pow(const Expr *lhs, LLVMValueRef rhs)
```

> Return expression for taking `lhs` to the power of `rhs`.

```c
static Expr gen_expr_ref(const AstNodeExpr *expr)
```

> Return reference to an expression `expr`.

```c
static Expr gen_expr_not(const Expr *lhs, LLVMValueRef rhs)
```

> Return expression for result of not operator for `rhs`.

```c
static Expr gen_expr_unary_neg(const Expr *lhs, LLVMValueRef rhs)
```

> Return expression for result of unary negation operator for `rhs`.

```c
static Expr gen_expr_is(const Expr *lhs, LLVMValueRef rhs)
```

> Return expression for result of is operator for `lhs` and `rhs`.

```c
static Expr gen_expr_is_str(LLVMValueRef lhs, LLVMValueRef rhs)
```

> Return expression for string-is operator against `lhs` and `rhs`.

```c
static Expr create_and_call_builtin_oper(const Type *rtype, LLVMTypeRef type, const char *name, LLVMValueRef lhs, LLVMValueRef rhs)
```

> Create a function called `name` (if it does not exist) which returns type
> `rtype`, and has operands of type `type`. Afterwards, call the new function
> with the `lhs` and `rhs` operands.

```c
static Expr gen_expr_is_not(const Expr *lhs, LLVMValueRef rhs)
```

> Return expression for result of is not operator for `lhs` and `rhs`.

```c
static Expr gen_expr_relational_oper(const Expr *lhs, LLVMValueRef rhs, LLVMIntPredicate int_pred, LLVMRealPredicate float_pred)
```

> Return result of relational comparison on `lhs` and `rhs`.
> \
> Depending on whether `type` is an int or float, compare `lhs` and `rhs` using
> `int_func` or `float_func`.

```c
static Expr gen_expr_less_than(const Expr *lhs, LLVMValueRef rhs)
```

> Return expression for result of less than operator for `lhs` and `rhs`.

```c
static Expr gen_expr_gtr_than(const Expr *lhs, LLVMValueRef rhs)
```

> Return expression for result of greater than operator for `lhs` and `rhs`.

```c
static Expr gen_expr_less_than_eq(const Expr *lhs, LLVMValueRef rhs)
```

> Return expression for result of less than or equal to operator for `lhs` and
> `rhs`.

```c
static Expr gen_expr_gtr_than_eq(const Expr *lhs, LLVMValueRef rhs)
```

> Return expression for result of greater than or equal to operator for `lhs`
> and `rhs`.

```c
static Expr gen_expr_logical_oper(const Expr *lhs, LLVMValueRef rhs, LLVMBuildX func)
```

> Return result of logical operation `func` on `lhs` and `rhs`.

```c
static Expr gen_expr_and(const Expr *lhs, LLVMValueRef rhs)
```

> Return result of logical "and" operation of `lhs` and `rhs`.

```c
static Expr gen_expr_or(const Expr *lhs, LLVMValueRef rhs)
```

> Return result of logical "or" operation of `lhs` and `rhs`.

```c
static Expr gen_expr_xor(const Expr *lhs, LLVMValueRef rhs)
```

> Return result of logical "xor" operation of `lhs` and `rhs`.

