#pragma once

#include <stdbool.h>
#include <string.h>
#include <wchar.h>

/*
Custom implementation of non-standard `strlcpy` function.
*/
char *strlcpy(char *dest, const char *src, size_t n) {
	char *ret=strncpy(dest, src, n);
	dest[n]='\0';

	return ret;
}

/*
Similar to above `strlcpy`, but for `wchar_t` types.

This function will always put a wide char NULL terminator at the end of the string.
*/
wchar_t *wcslcpy(wchar_t *dest, const wchar_t *src, size_t n) {
	wchar_t *ret=wcsncpy(dest, src, n);
	dest[n]=L'\0';

	return ret;
}

/*
Same as calling strcmp(s1, s2)==0
*/
bool samestr(const char *s1, const char *s2) {
	return strcmp(s1, s2)==0;
}