# skull/eval/types/float.c

```c
char *fmt_float_type(const Variable *const var)
```

> Returns the string representation of float `var`

```c
SkullFloat eval_float(const Token *const token, char32_t **error)
```

> Returns a Skull float parsed from `token`.
> \
> `error` is `NULL` if no error occurs, else `error` points to error msg.

