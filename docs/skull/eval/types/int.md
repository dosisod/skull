# skull/eval/types/int.c

```c
char *fmt_int_type(const Variable *const var)
```

> Returns the string representation of int `var`

```c
void *eval_integer(const Token *const token, char32_t **error)
```

> Converts a `TOKEN_INT_CONST` token to an integer pointer (`SkullInt *`).
> \
> `error` is `NULL` if no error occurs, else `error` points to error msg.

