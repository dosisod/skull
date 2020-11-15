#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "skull/common/errors.h"
#include "skull/common/malloc.h"
#include "skull/common/panic.h"

#include "skull/common/str.h"

/*
Make a heap allocated version of `str`.

The result of this function must be freed.
*/
char32_t *c32sdup(const char32_t *const str) {
	const size_t len = c32slen(str);

	char32_t *ret;
	ret = malloc((len + 1) * sizeof *ret);
	DIE_IF_MALLOC_FAILS(ret);

	c32sncpy(ret, str, len + 1);
	ret[len] = '\0';

	return ret;
}

/*
Concatenate `s1` and `s2`.

The result of this function must be freed.
*/
char32_t *c32scat(const char32_t *const s1, const char32_t *const s2) {
	const size_t len_s1 = c32slen(s1);
	const size_t len_s2 = c32slen(s2);

	char32_t *ret;
	ret = malloc((len_s1 + len_s2 + 1) * sizeof *ret);
	DIE_IF_MALLOC_FAILS(ret);

	c32sncpy(ret, s1, len_s1);
	c32sncpy(ret + len_s1, s2, len_s2 + 1);
	ret[len_s2 + len_s1] = '\0';

	return ret;
}

/*
char32_t equivalent of `strncpy`.

If there is room between the end of `src` and `dest[n]`, fill it with NULL.
*/
void c32sncpy(char32_t *dest, const char32_t *src, size_t n) {
	*dest = '\0';
	memset(dest, 0, n * sizeof *dest);

	while (*src && n > 0) {
		*dest = *src;
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
	if (!str) {
		return NULL;
	}

	//allocate the max that str could expand to
	char *ret = malloc((c32slen(str) + 1) * MB_CUR_MAX);
	DIE_IF_MALLOC_FAILS(ret);

	size_t offset = 0;
	static mbstate_t mbs;

	while (*str) {
		size_t length = c32rtomb(ret + offset, *str, &mbs);

		if ((length == 0) || (length > MB_CUR_MAX)) {
			break;
		}
		offset += length;
		str++;
	}

	//shrink string to only what we need
	ret = realloc(ret, offset + 1);
	DIE_IF_MALLOC_FAILS(ret);
	ret[offset] = '\0';

	return ret;
}

/*
Return the size of a UTF-32 string.
*/
__attribute__((pure)) size_t c32slen(const char32_t *str) {
	if (!str) {
		return 0;
	}

	size_t len = 0;

	while (*str) {
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
	char32_t *ret;
	ret = malloc((strlen(str) + 1) * sizeof *ret);
	DIE_IF_MALLOC_FAILS(ret);

	size_t offset = 0;

	mbstate_t mbs;
	memset(&mbs, 0, sizeof mbs);

	while (*str) {
		errno = 0;
		const size_t length = mbrtoc32(ret + offset, str, MB_CUR_MAX, &mbs);

		if (errno == EILSEQ) {
			printf("illegal UTF8 sequence at character offset %zu\n", offset);
			exit(1);
		}

		if ((length == 0) || (length > MB_CUR_MAX)) {
			break;
		}
		offset++;
		str += length;
	}

	//shrink string to only what we need
	ret = realloc(ret, (offset + 1) * sizeof *ret);
	DIE_IF_MALLOC_FAILS(ret);
	ret[offset] = '\0';

	return ret;
}

/*
Compare two UTF-32 strings `a` and `b`.

`a` and `b` must be of equal length, and match exactly.
*/
__attribute__((pure)) bool c32scmp(const char32_t *a, const char32_t *b) {
	while (*a && *b) {
		if (*a != *b) {
			return false;
		}
		a++;
		b++;
	}

	return !*a && !*b;
}

/*
Compare at most `n` chars of two UTF-32 strings, `a` and `b`.
*/
__attribute__((pure)) bool c32sncmp(const char32_t *a, const char32_t *b, size_t n) {
	while (*a && *b && n > 0) {
		if (*a != *b) {
			return false;
		}
		a++;
		b++;
		n--;
	}
	return n == 0;
}

/*
Return pointer to first occurence of `c` in `str`.

If it cannot be found, NULL is returned instead.
*/
__attribute__((pure)) const char32_t *c32schr(const char32_t *str, char32_t c) {
	while (*str) {
		if (*str == c) {
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
		((c >= '0') && (c <= '9')) ||
		((c >= 'a') && (c <= 'f')) ||
		((c >= 'A') && (c <= 'F'))
	);
}

/*
Returns whether the UTF-32 character `c` is an alpha numeric character.
*/
__attribute__((const)) bool c32isalnum(char32_t c) {
	return (
		c32isdigit(c) ||
		((c >= 'A') && (c <= 'Z')) ||
		((c >= 'a') && (c <= 'z'))
	);
}

/*
Return whether the UTF-32 character `c` is a digit.
*/
__attribute__((const)) bool c32isdigit(char32_t c) {
	return c >= '0' && c <= '9';
}

/*
Return last occurence of `sub` in string `str`.

`NULL` is returned if no such string is found.
*/
const char __attribute__((pure)) *strrstr(const char *const str, const char *const sub) {
	const size_t str_len = strlen(str);
	const size_t sub_len = strlen(sub);

	const char *look = str + str_len - sub_len;

	while (look >= str) {
		if (strncmp(look, sub, sub_len) == 0) {
			return look;
		}
		look--;
	}
	return NULL;
}

/*
Returns the unescaped version of an escaped character starting at `str`, or NULL character.
*/
char32_t c32sunescape(const char32_t **str_) {
	const char32_t *str = *str_;

	if (*str != '\\') {
		return '\0';
	}

	(*str_)++;

	char32_t escape = str[1];
	char32_t opt1 = '\0';
	char32_t opt2 = '\0';

	if (escape == '\\') {
		return '\\';
	}
	if (escape == '0') {
		return '\0';
	}
	if (escape == 'e') {
		return '\033';
	}
	if (escape == 't') {
		return '\t';
	}
	if (escape == 'r') {
		return '\r';
	}
	if (escape == 'n') {
		return '\n';
	}
	if (escape == 'x' && str[2]) {
		*str_ += 2;

		opt1 = str[2];
		opt2 = str[3];

		if (c32isxdigit(str[2]) && c32isxdigit(str[3])) {
			char hex[3] = { (char)opt1, (char)opt2, '\0' };
			return (char32_t)strtol(hex, NULL, 16);
		}

		if (opt1 == '\'' || opt1 == '\"') {
			opt1 = '\0';
		}
		else if (opt2 == '\'' || opt2 == '\"') {
			opt2 = '\0';
		}
	}

	if (escape == '\'' || escape == '\"') {
		escape = '\0';
	}

	const char32_t bad_escape[5] = { '\\', escape, opt1, opt2, '\0' };
	PANIC(ERR_BAD_ESCAPE, { .str = bad_escape });
}
