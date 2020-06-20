#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
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

/*
Return the size of a UTF-32 string.
*/
size_t c32slen(const char32_t *str) {
	size_t len=0;

	while (*str!=U'\0') {
		str++;
		len++;
	}

	return len;
}

/*
Convert a UTF-32 string `str` into a multi-byte string (probably UTF-8).

The result of this function must be freed.
*/
char *c32stombs(const char32_t *str) {
	//allocate the max that str could expand to
	char *ret=malloc((c32slen(str) + 1) * MB_CUR_MAX);

	size_t offset=0;
	static mbstate_t mbs;

	while (*str!=U'\0') {
		size_t length=c32rtomb(ret+offset, *str, &mbs);

		if ((length==0) || (length>MB_CUR_MAX)) {
			break;
		}
		offset+=length;
		str++;
	}

	//shrink string to only what we need
	ret=realloc(ret, offset+1);
	ret[offset]='\0';

	return ret;
}

/*
Convert a multi-byte string `str` (probably UTF-8) into a UTF-32 string.

The result of this function must be freed.
*/
char32_t *mbstoc32s(const char *str) {
	//allocate the max that str could expand to
	char32_t *ret=malloc((strlen(str) + 1) * sizeof(char32_t));

	size_t offset=0;
	static mbstate_t mbs;

	while (*str!='\0') {
		size_t length=mbrtoc32(ret+offset, str, MB_CUR_MAX, &mbs);

		if ((length==0) || (length>MB_CUR_MAX)) {
			break;
		}
		offset+=length;
		str++;
	}

	//shrink string to only what we need
	ret=realloc(ret, sizeof(char32_t) * (offset+1));
	ret[offset]=U'\0';

	return ret;
}
