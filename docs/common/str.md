# src/common/str.h

#### `char *strlcpy(char *dest, const char *src, size_t n)`
Copy `n - 1` characters to `dest` from `src`, ensuring the last character is NULL terminated.

#### `wchar_t *wcslcpy(wchar_t *dest, const wchar_t *src, size_t n)`
Similar to above `strlcpy`, but for `wchar_t` types.

#### `wchar_t *wcsdup(const wchar_t *str)`
Make a heap allocated version of `str`.

The result of this function must be freed.

#### `size_t c32slen(const char32_t *str)`
Return the size of a UTF-32 string.

#### `char *c32stombs(const char32_t *str)`
Convert a UTF-32 string `str` into a multi-byte string (probably UTF-8).

The result of this function must be freed.

#### `char32_t *mbstoc32s(const char *str)`
Convert a multi-byte string `str` (probably UTF-8) into a UTF-32 string.

The result of this function must be freed.

