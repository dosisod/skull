# skull/common/str

```c
char32_t *c32sdup(const char32_t *const str)
```

> Make a heap allocated version of `str`.
> \
> The result of this function must be freed.

```c
void c32sncpy(char32_t *dest, const char32_t *src, size_t n)
```

> char32_t equivalent of `strncpy`.
> \
> If there is room between the end of `src` and `dest[n]`, fill it with NULL.

```c
char *c32stombs(const char32_t *str, const Location *location)
```

> Convert a UTF-32 string `str` into a multi-byte string (probably UTF-8).
> \
> Optionaly load use `location` for printing error.
> \
> The result of this function must be freed.

```c
__attribute__((pure)) size_t c32slen(const char32_t *str)
```

> Return the size of a UTF-32 string.

```c
char32_t *mbstoc32s(const char *str)
```

> Convert a multi-byte string `str` (probably UTF-8) into a UTF-32 string.
> \
> The result of this function must be freed.

```c
__attribute__((pure)) bool c32scmp(const char32_t *a, const char32_t *b)
```

> Compare two UTF-32 strings `a` and `b`.
> \
> `a` and `b` must be of equal length, and match exactly.

```c
__attribute__((pure)) bool c32sncmp(const char32_t *a, const char32_t *b, size_t n)
```

> Compare at most `n` chars of two UTF-32 strings, `a` and `b`.

```c
__attribute__((const)) bool c32isxdigit(char32_t c)
```

> Return whether the UTF-32 character `c` is a hexidecimal digit.

```c
__attribute__((const)) bool c32isalnum(char32_t c)
```

> Returns whether the UTF-32 character `c` is an alpha numeric character.

```c
__attribute__((const)) bool c32isdigit(char32_t c)
```

> Return whether the UTF-32 character `c` is a digit.

```c
const char __attribute__((pure)) *strrstr(const char *const str, const char *const sub)
```

> Return last occurence of `sub` in string `str`.
> \
> `NULL` is returned if no such string is found.

```c
__attribute__((const)) char32_t c32unhex(char32_t c)
```

> Convert `c` as an ASCII hex value to an integer.

```c
char32_t c32sunescape(const char32_t **str_, const char32_t **error)
```

> Returns the unescaped version of an escaped character starting at `str`, or
> NULL character.

```c
__attribute__ ((format(printf, 1, 2))) char *uvsnprintf(const char *const fmt, ...)
```

> Useful `vsnprintf` function (formats `...` based on `fmt`).
> \
> Result of function must be freed.
> \
> `fmt` MUST be a constant, non-user supplied string (ie, printf safe).

