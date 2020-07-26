#include <stdlib.h>
#include <string.h>

#include "skull/common/malloc.h"

#include "skull/common/str.h"

/*
Make a heap allocated version of `str`.

The result of this function must be freed.
*/
char32_t *c32sdup(const char32_t *str) {
	size_t len=c32slen(str);

	char32_t *ret=malloc(sizeof(char32_t) * (len + 1));
	DIE_IF_MALLOC_FAILS(ret);

	c32slcpy(ret, str, len + 1);

	return ret;
}

/*
Concatenate `s1` and `s2`.

The result of this function must be freed.
*/
char32_t *c32scat(const char32_t *s1, const char32_t *s2) {
	size_t len_s1=c32slen(s1);
	size_t len_s2=c32slen(s2);

	char32_t *ret=malloc(sizeof(char32_t) * (len_s1 + len_s2 + 1));
	DIE_IF_MALLOC_FAILS(ret);

	c32sncpy(ret, s1, len_s1);
	c32slcpy(ret + len_s1, s2, len_s2 + 1);

	return ret;
}

/*
Similar to `strlcpy`, but for `char32_t` types.
*/
void c32slcpy(char32_t *dest, const char32_t *src, size_t n) {
	c32sncpy(dest, src, n);
	dest[n - 1]=U'\0';
}

/*
char32_t equivalent of `strncpy`.

If there is room between the end of `src` and `dest[n]`, fill it with NULL.
*/
void c32sncpy(char32_t *dest, const char32_t *src, size_t n) {
	memset(dest, 0, n * sizeof(char32_t));

	while (*src!=U'\0' && n>0) {
		*dest=*src;
		src++;
		dest++;
		n--;
	}
}

/*
Convert a UTF-32 string `str` into a multi-byte string (probably UTF-8).

The result of this function must be freed.
*/
char *c32stombs(const char32_t *str) {
	if (str==NULL) {
		return NULL;
	}

	//allocate the max that str could expand to
	char *ret=malloc((c32slen(str) + 1) * MB_CUR_MAX);
	DIE_IF_MALLOC_FAILS(ret);

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
Return the size of a UTF-32 string.
*/
__attribute__((pure)) size_t c32slen(const char32_t *str) {
	size_t len=0;

	while (*str!=U'\0') {
		str++;
		len++;
	}

	return len;
}

/*
Convert a multi-byte string `str` (probably UTF-8) into a UTF-32 string.

The result of this function must be freed.
*/
char32_t *mbstoc32s(const char *str) {
	//allocate the max that str could expand to
	char32_t *ret=malloc((strlen(str) + 1) * sizeof(char32_t));
	DIE_IF_MALLOC_FAILS(ret);

	size_t offset=0;

	mbstate_t mbs;
	memset(&mbs, 0, sizeof(mbstate_t));

	while (*str!='\0') {
		size_t length=mbrtoc32(ret+offset, str, MB_CUR_MAX, &mbs);

		if ((length==0) || (length>MB_CUR_MAX)) {
			break;
		}
		offset++;
		str+=length;
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
__attribute__((pure)) bool c32scmp(const char32_t *a, const char32_t *b) {
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
__attribute__((pure)) bool c32sncmp(const char32_t *a, const char32_t *b, size_t n) {
	while (*a!=U'\0' && *b!=U'\0' && n>0) {
		if (*a != *b) {
			return false;
		}
		a++;
		b++;
		n--;
	}
	return n==0;
}

/*
Return pointer to first occurence of `c` in `str`.

If it cannot be found, NULL is returned instead.
*/
__attribute__((pure)) const char32_t *c32schr(const char32_t *str, char32_t c) {
	while (*str!=U'\0') {
		if (*str==c) {
			return str;
		}
		str++;
	}
	return NULL;
}

/*
Return whether the UTF-32 character `c` is a hexidecimal digit.
*/
__attribute__((const)) bool c32isxdigit(char32_t c) {
	return (
		((c >= U'0') && (c <= U'9')) ||
		((c >= U'a') && (c <= U'f')) ||
		((c >= U'A') && (c <= U'F'))
	);
}

/*
Returns whether the UTF-32 character `c` is an alpha numeric character.
*/
__attribute__((const)) bool c32isalnum(char32_t c) {
	return (
		c32isdigit(c) ||
		((c >= U'A') && (c <= U'Z')) ||
		((c >= U'a') && (c <= U'z'))
	);
}

/*
Return whether the UTF-32 character `c` is a digit.
*/
__attribute__((const)) bool c32isdigit(char32_t c) {
	return (c >= U'0') && (c <= U'9');
}

/*
Return last occurence of `sub` in string `str`.

`NULL` is returned if no such string is found.
*/
const char *strrstr(const char *str, const char *sub) {
	size_t str_len=strlen(str);
	size_t sub_len=strlen(sub);

	const char *look=str + str_len - sub_len;

	while (look >= str) {
		if (strncmp(look, sub, sub_len)==0) {
			return look;
		}
		look--;
	}
	return NULL;
}
