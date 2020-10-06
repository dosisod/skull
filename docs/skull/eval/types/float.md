# skull/eval/types/float.c

```c
char32_t *fmt_float_type(const Variable *const var)
```

> Returns the string representation of float `var`

```c
Variable *add_float_type(const Variable *const lhs, const Variable *const rhs)
```

> Add `lhs` and `rhs` floats together

```c
Variable *sub_float_type(const Variable *const lhs, const Variable *const rhs)
```

> Subtract `rhs` float from `lhs` float

```c
Variable *div_float_type(const Variable *const lhs, const Variable *const rhs)
```

> Divide `lhs` float by `rhs` float

```c
Variable *mult_float_type(const Variable *const lhs, const Variable *const rhs)
```

> Multiply `lhs` and `rhs` floats together

```c
void *eval_float(const Token *const token, const char32_t **error)
```

> Converts a `TOKEN_FLOAT_CONST` token to a floating point number pointer (`SkullFloat *`).
> \
> `error` is `NULL` if no error occurs, else `error` points to error msg.

