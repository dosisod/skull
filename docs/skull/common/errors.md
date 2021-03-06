# skull/common/errors

```c
typedef struct {
	const char *str;
	const char32_t *str32;
	const Token *tok;
	const Location *loc;
	const Variable *var;
	Type type;
	char *real; // copy of string that will be used in final error msg

	size_t i;
}
```

> `ErrorMsg` stores a reference to a given object, generating an error msg based
> on which one is set.
> \
> See `fmt_error_stringify` for uses of these fields.

```c
static bool do_show_color(void)
```

> Return whether color output should be displayed.

```c
char *fmt_error(ErrorType type, ErrorCode error_code, ErrorMsg msgs[])
```

> Returns formatted error message.
> \
> Every `%s` in the string is expanded according to the corresponding `ErrorMsg`
> in `msgs`.

```c
void fmt_error_stringify(ErrorMsg *const msg)
```

> Convert error msg `msg` for use in `fmt_error`.
> \
> Depending on whether `msg` is a token, a variable, or a string, the resulting
> feild `real` will be created differently.
> \
> If `i` (integer) is specified for `msg`, it must be one more then the actual
> value. This is due to `0` being falsey, and thus not being able to be checked.

