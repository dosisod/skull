# src/common/str.h

#### `char *strlcpy(char *dest, const char *src, size_t n)`
Copy `n - 1` characters to `dest` from `src`, ensuring the last character is NULL terminated.

#### `wchar_t *wcslcpy(wchar_t *dest, const wchar_t *src, size_t n)`
Similar to above `strlcpy`, but for `wchar_t` types.

#### `wchar_t *wcsdup(const wchar_t *str)`
Make a heap allocated version of `str`.

The result of this function must be freed.

