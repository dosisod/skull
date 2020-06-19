#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

//Android ships with the below functions, dont redefine if thats the case.
//Other platforms may need to be added in the future
#ifndef __ANDROID_API__

/*
Copy `n - 1` characters to `dest` from `src`, ensuring the last character is NULL terminated.
*/
char *strlcpy(char *dest, const char *src, size_t n) {
	char *ret=strncpy(dest, src, n - 1);
	dest[n - 1]='\0';

	return ret;
}

/*
Similar to above `strlcpy`, but for `wchar_t` types.
*/
wchar_t *wcslcpy(wchar_t *dest, const wchar_t *src, size_t n) {
	wchar_t *ret=wcsncpy(dest, src, n - 1);
	dest[n - 1]=L'\0';

	return ret;
}

/*
Make a heap allocated version of `str`.

The result of this function must be freed.
*/
wchar_t *wcsdup(const wchar_t *str) {
	size_t len=wcslen(str);

	wchar_t *ret=malloc(sizeof(wchar_t) * (len + 1));
	wcsncpy(ret, str, len);
	ret[len]=L'\0';

	return ret;
}

#endif