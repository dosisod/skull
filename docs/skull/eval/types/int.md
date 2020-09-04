# skull/eval/types/int.c

```c
char32_t *fmt_int_type(const Variable *var)
```

> Returns the string representation of int `var`

```c
Variable *add_int_type(const Variable *lhs, const Variable *rhs)
```

> Add `lhs` and `rhs` ints together

```c
Variable *sub_int_type(const Variable *lhs, const Variable *rhs)
```

> Subtract `rhs` int from `lhs` int

```c
Variable *div_int_type(const Variable *lhs, const Variable *rhs)
```

> Divide `lhs` int by `rhs` int
> \
> If `rhs` is zero, return NULL.

```c
Variable *mult_int_type(const Variable *lhs, const Variable *rhs)
```

> Multiply `lhs` and `rhs` ints together

