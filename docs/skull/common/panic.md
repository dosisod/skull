# skull/common/panic

```c
#define PANIC(fmt, ...) \
	fmt_error(fmt, (ErrorMsg[]){ \
		__VA_ARGS__, \
		{0} \
	}); \
	exit(1)
```

> Panic (exit with error msg), using `fmt` as the message, and `...` as the
> `ErrorMsg` parameters.

