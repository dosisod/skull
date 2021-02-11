# skull/common/errors

```c
typedef struct {
	const char32_t *str;
	const Token *tok;
	const Variable *var;
	const Type *type;
	size_t i;

	// copy of string that will be used in final error msg
	char *real;
}
```

> `ErrorMsg` stores a reference to a given object, generating an error msg based
> on which one is set.
> \
> See `fmt_error_stringify` for uses of these fields.

```c
void fmt_error(const char *const fmt, ErrorMsg msgs[])
```

> Prints formatted an error message.
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

