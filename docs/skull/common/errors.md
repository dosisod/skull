# skull/common/errors

```c
void fmt_error(const char *const fmt, ErrorMsg msgs[])
```

> Prints formatted an error message.
> \
> Every `%s` in the string is expanded according to the corresponding `ErrorMsg` in `msgs`.

```c
void fmt_error_stringify(ErrorMsg *const msg)
```

> Convert error msg `msg` for use in `fmt_error`.
> \
> Depending on whether `msg` is a token, a variable, or a string, the resulting feild `real` will be created differently.

