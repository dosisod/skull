# skull/eval/types/int.c

```c
char *fmt_int_type(const Variable *const var)
```

> Returns the string representation of int `var`

```c
SkullInt eval_integer(const Token *const token, char32_t **error)
```

> Returns an Skull integer parsed from `token`.
> \
> `error` is `NULL` if no error occurs, else `error` points to error msg.

