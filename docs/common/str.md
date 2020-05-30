# src/common/str.h

#### `char *strlcpy(char *dest, const char *src, size_t n)`
Custom implementation of non-standard `strlcpy` function.

This function will always put a wide char NULL terminator at the end of the string.

#### `wchar_t *wcslcpy(wchar_t *dest, const wchar_t *src, size_t n)`
Similar to above `strlcpy`, but for `wchar_t` types.

This function will always put a wide char NULL terminator at the end of the string.

