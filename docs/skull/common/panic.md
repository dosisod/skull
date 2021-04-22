# skull/common/panic

```c
#define PANIC(fmt, ...) \
	FMT_ERROR(fmt, __VA_ARGS__); \
	exit(1)
```

> Panic (exit with error msg), using `fmt` as the message, and `...` as the
> `ErrorMsg` parameters.

