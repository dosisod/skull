# src/common/str.h

#### `char *strlcpy(char *dest, const char *src, size_t n)`
Custom implementation of non-standard `strlcpy` function.

#### `wchar_t *wcslcpy(wchar_t *dest, const wchar_t *src, size_t n)`
Similar to above `strlcpy`, but for `wchar_t` types.

This function will always put a wide char NULL terminator at the end of the string.

#### `bool samestr(const char *s1, const char *s2)`
Same as calling strcmp(s1, s2)==0

