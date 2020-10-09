# skull/eval/types/float.c

```c
char *fmt_float_type(const Variable *const var)
```

> Returns the string representation of float `var`

```c
void *eval_float(const Token *const token, const char32_t **error)
```

> Converts a `TOKEN_FLOAT_CONST` token to a floating point number pointer (`SkullFloat *`).
> \
> `error` is `NULL` if no error occurs, else `error` points to error msg.

