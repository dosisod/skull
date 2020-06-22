# src/common/str.h

#### `char *strlcpy(char *dest, const char *src, size_t n)`
Copy `n - 1` characters to `dest` from `src`, ensuring the last character is NULL terminated.

#### `void c32sncpy(char32_t *dest, const char32_t *src, size_t n)`
char32_t equivalent of `strncpy`.

If there is room between the end of `src` and `dest[n]`, fill it with NULL.

#### `void c32slcpy(char32_t *dest, const char32_t *src, size_t n)`
Similar to above `strlcpy`, but for `char32_t` types.

#### `size_t c32slen(const char32_t *str)`
Return the size of a UTF-32 string.

#### `char32_t *c32sdup(const char32_t *str)`
Make a heap allocated version of `str`.

The result of this function must be freed.

#### `char *c32stombs(const char32_t *str)`
Convert a UTF-32 string `str` into a multi-byte string (probably UTF-8).

The result of this function must be freed.

#### `char32_t *mbstoc32s(const char *str)`
Convert a multi-byte string `str` (probably UTF-8) into a UTF-32 string.

The result of this function must be freed.

#### `bool c32scmp(const char32_t *a, const char32_t *b)`
Compare two UTF-32 strings `a` and `b`.

`a` and `b` must be of equal length, and match exactly.

#### `bool c32sncmp(const char32_t *a, const char32_t *b, size_t n)`
Compare at most `n` chars of two UTF-32 strings, `a` and `b`.

#### `const char32_t *c32schr(const char32_t *str, char32_t c)`
Return pointer to first occurence of `c` in `str`.

If it cannot be found, NULL is returned instead.

#### `bool c32isdigit(char32_t c)`
Return whether the UTF-32 character `c` is a digit.

#### `bool c32isxdigit(char32_t c)`
Return whether the UTF-32 character `c` is a hexidecimal digit.

#### `bool c32isalnum(char32_t c)`
Returns whether the UTF-32 character `c` is an alpha numeric character.

