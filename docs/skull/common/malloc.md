# skull/common/malloc

```c
#define DIE_IF_MALLOC_FAILS(ptr) \
	if (!(ptr)) { \
		puts("\nfailed to allocate sufficient memory, exiting"); \
		exit(1); \
	}
```

> Exit if malloc fails to allocate memory.

