# skull/eval/types/str.c

```c
char32_t *fmt_str_type(const Variable *var)
```

> Returns the string representation of string `var`

```c
Variable *add_str_type(const Variable *lhs, const Variable *rhs)
```

> Concatenate `lhs` and `rhs` strings

```c
void *eval_str(const Token *token, const char32_t **error)
```

> Return pointer to string, converted from `token`

