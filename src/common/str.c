#include <stdlib.h>
#include <string.h>

#include "malloc.h"

#include "str.h"

void c32sncpy(char32_t *dest, const char32_t *src, size_t n) {
	memset(dest, 0, n * sizeof(char32_t));

	while (*src!=U'\0' && n>0) {
		*dest=*src;
		src++;
		dest++;
		n--;
	}
}

void c32slcpy(char32_t *dest, const char32_t *src, size_t n) {
	c32sncpy(dest, src, n);
	dest[n - 1]=U'\0';
}

__attribute__((pure)) size_t c32slen(const char32_t *str) {
	size_t len=0;

	while (*str!=U'\0') {
		str++;
		len++;
	}

	return len;
}

char32_t *c32sdup(const char32_t *str) {
	size_t len=c32slen(str);

	char32_t *ret=malloc(sizeof(char32_t) * (len + 1));
	DIE_IF_MALLOC_FAILS(ret);

	c32slcpy(ret, str, len + 1);

	return ret;
}

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

__attribute__((pure)) bool c32sncmp(const char32_t *a, const char32_t *b, size_t n) {
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

__attribute__((pure)) const char32_t *c32schr(const char32_t *str, char32_t c) {
	while (*str!=U'\0') {
		if (*str==c) {
			return str;
		}
		str++;
	}
	return NULL;
}

__attribute__((const)) bool c32isdigit(char32_t c) {
	return (c >= U'0') && (c <= U'9');
}

__attribute__((const)) bool c32isxdigit(char32_t c) {
	return (
		((c >= U'0') && (c <= U'9')) ||
		((c >= U'a') && (c <= U'f')) ||
		((c >= U'A') && (c <= U'F'))
	);
}

__attribute__((const)) bool c32isalnum(char32_t c) {
	return (
		c32isdigit(c) ||
		((c >= U'A') && (c <= U'Z')) ||
		((c >= U'a') && (c <= U'z'))
	);
}
