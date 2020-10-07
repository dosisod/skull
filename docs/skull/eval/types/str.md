# skull/eval/types/str.c

```c
char *fmt_str_type(const Variable *const var)
```

> Returns the string representation of string `var`

```c
Variable *add_str_type(const Variable *const lhs, const Variable *const rhs)
```

> Concatenate `lhs` and `rhs` strings

```c
void *eval_str(const Token *const token, const char32_t **error)
```

> Return pointer to string, converted from `token`

