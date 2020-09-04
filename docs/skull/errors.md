# skull/errors.c

```c
char32_t *fmt_error(const char32_t *fmt, ErrorMsg msgs[])
```

> Format an error message.
> \
> Every `%` in the string is expanded according to the corresponding `ErrorMsg` in `msgs`.
> \
> The result of this function must be freed.

```c
void fmt_error_stringify(ErrorMsg *msg)
```

> Convert error msg `msg` for use in `fmt_error`.
> \
> Depending on whether `msg` is a token, a variable, or a string, the resulting feild `real` will be created differently.

```c
__attribute__((pure)) bool is_error_msg(const char32_t *str)
```

> Returns true if `str` is an error msg.

