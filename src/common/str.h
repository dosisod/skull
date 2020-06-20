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
Similar to above `strlcpy`, but for `wchar_t` types.
*/
wchar_t *wcslcpy(wchar_t *dest, const wchar_t *src, size_t n) {
	wchar_t *ret=wcsncpy(dest, src, n - 1);
	dest[n - 1]=L'\0';

	return ret;
}

/*
char32_t equivalent of `strncpy`, does not add NULL terminator.
*/
void c32sncpy(char32_t *dest, const char32_t *src, size_t n) {
	size_t offset=0;

	while (*src!=U'\0' && n>0) {
		dest[offset]=*src;

		src++;
		offset++;
		n--;
	}
}

/*
Similar to above `strlcpy`, but for `char32_t` types.
*/
void c32slcpy(char32_t *dest, const char32_t *src, size_t n) {
	c32sncpy(dest, src, n);
	dest[n - 1]=L'\0';
}

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
Make a heap allocated version of `str`.

The result of this function must be freed.
*/
char32_t *c32sdup(const char32_t *str) {
	size_t len=c32slen(str);

	char32_t *ret=malloc(sizeof(char32_t) * (len + 1));
	c32slcpy(ret, str, len + 1);

	return ret;
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

/*
Compare two UTF-32 strings `a` and `b`.

`a` and `b` must be of equal length, and match exactly.
*/
bool c32scmp(const char32_t *a, const char32_t *b) {
	while (*a!=U'\0' && *b!=U'\0') {
		if (*a != *b) {
			return false;
		}
		a++;
		b++;
	}

	return (*a==U'\0' && *b==U'\0');
}

/*
Compare at most `n` chars of two UTF-32 strings, `a` and `b`.
*/
bool c32sncmp(const char32_t *a, const char32_t *b, size_t n) {
	while (*a!=U'\0' && *b!=U'\0' && n>0) {
		if (*a != *b) {
			return false;
		}
		a++;
		b++;
		n--;
	}
	return true;
}

/*
Return pointer to first occurence of `c` in `str`.

If it cannot be found, NULL is returned instead.
*/
const char32_t *c32schr(const char32_t *str, char32_t c) {
	while (*str!=U'\0') {
		if (*str==c) {
			return str;
		}
		str++;
	}
	return NULL;
}

/*
Return whether the UTF-32 character `c` is a digit.
*/
bool c32isdigit(char32_t c) {
	return (c >= U'0') && (c <= U'9');
}

/*
Return whether the UTF-32 character `c` is a hexidecimal digit.
*/
bool c32isxdigit(char32_t c) {
	return (
		((c >= U'0') && (c <= U'9')) ||
		((c >= U'a') && (c <= U'f')) ||
		((c >= U'A') && (c <= U'F'))
	);
}

/*
Returns whether the UTF-32 character `c` is an alpha numeric character.
*/
bool c32isalnum(char32_t c) {
	return (
		c32isdigit(c) ||
		((c >= U'A') && (c <= U'Z')) ||
		((c >= U'a') && (c <= U'z'))
	);
}
