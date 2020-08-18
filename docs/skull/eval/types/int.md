# skull/eval/types/int.c

```c
char32_t *fmt_int_type(const variable_t *var)
```

> Returns the string representation of int `var`

```c
variable_t *add_int_type(const variable_t *lhs, const variable_t *rhs)
```

> Add `lhs` and `rhs` ints together

```c
variable_t *sub_int_type(const variable_t *lhs, const variable_t *rhs)
```

> Subtract `rhs` int from `lhs` int

```c
variable_t *div_int_type(const variable_t *lhs, const variable_t *rhs)
```

> Divide `lhs` int by `rhs` int
> \
> If `rhs` is zero, return NULL.

```c
variable_t *mult_int_type(const variable_t *lhs, const variable_t *rhs)
```

> Multiply `lhs` and `rhs` ints together

