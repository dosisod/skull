# skull/common/errors

```c
typedef struct {
	const char *str;
	const char32_t *str32;
	const Token *tok;
	const Location *loc;
	const Variable *var;
	Type *type;
	char *real; // copy of string that will be used in final error msg

	size_t i;
}
```

> `Message` allows for passing different objects (strings, types, variables),
> and have the proper string representations generated automatically.
> \
> See `message_stringify` for uses of these fields.

```c
static char *fmt_message_internal(ErrorType type, ErrorCode id, Message msgs[])
```

> Returns formatted message.
> \
> Every `%s` in the string is expanded according to the corresponding `Message`
> in `msgs`.

```c
void message_stringify(Message *const msg)
```

> Convert error msg `msg` for use in `fmt_message`.
> \
> Depending on whether `msg` is a token, a variable, or a string, the resulting
> feild `real` will be created differently.
> \
> If `i` (integer) is specified for `msg`, it must be one more then the actual
> value. This is due to `0` being falsey, and thus not being able to be checked.

