# skull/common/local

```c
#define SETUP_LOCALE() \
	static_assert(__STDC_UTF_32__, \
		"Your system does not support UTF-32 for 32 bit char types, we" \
		"cannot ensure Unicode operations will be handled correctly" \
	); \
	const char *loc = ""; \
	if (strcmp("C", setlocale(LC_CTYPE, "")) == 0) { \
		loc = "en_US.UTF8"; \
	} \
	if (!setlocale(LC_CTYPE, loc)) { \
		puts("cannot set type"); \
	}
```

> Try setup locale with UTF-32 and UTF-8 support.

