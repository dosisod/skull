#pragma once

#include <stdbool.h>
#include <string.h>
#include <wchar.h>

//Android ships with the below functions, dont redefine if thats the case.
//Other platforms may need to be added in the future
#ifndef __ANDROID_API__

/*
Custom implementation of non-standard `strlcpy` function.

This function will always put a NULL terminator at the end of the string.
*/
char *strlcpy(char *dest, const char *src, size_t n) {
	char *ret=strncpy(dest, src, n - 1);
	dest[n - 1]='\0';

	return ret;
}

/*
Similar to above `strlcpy`, but for `wchar_t` types.

This function will always put a wide char NULL terminator at the end of the string.
*/
wchar_t *wcslcpy(wchar_t *dest, const wchar_t *src, size_t n) {
	wchar_t *ret=wcsncpy(dest, src, n - 1);
	dest[n - 1]=L'\0';

	return ret;
}

#endif