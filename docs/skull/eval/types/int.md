# skull/eval/types/int.c

```c
char *fmt_int_type(const Variable *const var)
```

> Returns the string representation of int `var`

```c
Variable *add_int_type(const Variable *const lhs, const Variable *const rhs)
```

> Add `lhs` and `rhs` ints together

```c
Variable *sub_int_type(const Variable *const lhs, const Variable *const rhs)
```

> Subtract `rhs` int from `lhs` int

```c
Variable *div_int_type(const Variable *const lhs, const Variable *const rhs)
```

> Divide `lhs` int by `rhs` int
> \
> If `rhs` is zero, return NULL.

```c
Variable *mult_int_type(const Variable *const lhs, const Variable *const rhs)
```

> Multiply `lhs` and `rhs` ints together

```c
void *eval_integer(const Token *const token, const char32_t **error)
```

> Converts a `TOKEN_INT_CONST` token to an integer pointer (`SkullInt *`).
> \
> `error` is `NULL` if no error occurs, else `error` points to error msg.

