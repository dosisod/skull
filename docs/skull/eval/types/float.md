# skull/eval/types/float.c

```c
char32_t *fmt_float_type(const Variable *var)
```

> Returns the string representation of float `var`

```c
Variable *add_float_type(const Variable *lhs, const Variable *rhs)
```

> Add `lhs` and `rhs` floats together

```c
Variable *sub_float_type(const Variable *lhs, const Variable *rhs)
```

> Subtract `rhs` float from `lhs` float

```c
Variable *div_float_type(const Variable *lhs, const Variable *rhs)
```

> Divide `lhs` float by `rhs` float

```c
Variable *mult_float_type(const Variable *lhs, const Variable *rhs)
```

> Multiply `lhs` and `rhs` floats together

