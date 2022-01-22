# skull/common/malloc

```c
#define DIE_IF_MALLOC_FAILS(ptr) \
	if (!(ptr)) { \
		fprintf(stderr, "\nfailed to allocate sufficient memory, exiting\n"); \
		exit(1); \
	}
```

> Exit if malloc fails to allocate memory.

```c
void *Malloc(size_t size)
```

> `NULL` safe wrapper for `malloc`

```c
void *Calloc(size_t nmemb, size_t size)
```

> `NULL` safe wrapper for `calloc`

```c
void *Realloc(void *ptr, size_t size)
```

> `NULL` safe wrapper for `realloc`

