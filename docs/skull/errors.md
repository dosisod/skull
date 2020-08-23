# skull/errors.c

```c
char32_t *fmt_error(const char32_t *fmt, const char32_t *strs[])
```

> Format an error message.
> \
> Every `%` in the string is replaced with the corresponding string in `strs`.
> \
> The result of this function must be freed.

```c
__attribute__((pure)) bool is_error_msg(const char32_t *str)
```

> Returns true if `str` is an error msg.

