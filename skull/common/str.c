#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
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
	ret = Malloc((len + 1) * sizeof *ret);

	c32sncpy(ret, str, len + 1);
	ret[len] = '\0';

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
	char *ret = Malloc((c32slen(str) + 1) * MB_CUR_MAX);

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
	ret = Realloc(ret, offset + 1);
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
	ret = Malloc((strlen(str) + 1) * sizeof *ret);

	size_t offset = 0;

	mbstate_t mbs;
	memset(&mbs, 0, sizeof mbs);

	while (*str) {
		errno = 0;
		const size_t length = mbrtoc32(ret + offset, str, MB_CUR_MAX, &mbs);

		if (errno == EILSEQ) {
			PANIC(ERR_ILLEGAL_SEQ, { .i = offset + 1 });
		}

		if ((length == 0) || (length > MB_CUR_MAX)) {
			break;
		}
		offset++;
		str += length;
	}

	//shrink string to only what we need
	ret = Realloc(ret, (offset + 1) * sizeof *ret);
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
__attribute__((pure)) bool c32sncmp(
	const char32_t *a,
	const char32_t *b,
	size_t n
) {
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
const char __attribute__((pure)) *strrstr(
	const char *const str,
	const char *const sub
) {
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
Returns the unescaped version of an escaped character starting at `str`, or
NULL character.
*/
char32_t c32sunescape(const char32_t **str_, const char32_t **error) {
	const char32_t *str = *str_;

	if (*str != '\\') {
		return '\0';
	}

	char32_t escape = str[1];
	char32_t opt1 = '\0';
	char32_t opt2 = '\0';

	(*str_)++;

switch (escape) {
	case '\\': return '\\';
	case '\"': return '\"';
	case '\'': return '\'';
	case '0': return '\0';
	case 'e': return '\033';
	case 't': return '\t';
	case 'r': return '\r';
	case 'n': return '\n';
	default: break;
}

	if (escape == 'x' && str[2]) {
		*str_ += 2;

		opt1 = str[2];
		opt2 = str[3];

		if (c32isxdigit(str[2]) && c32isxdigit(str[3])) {
			return ((opt1 - 0x30) << 4) + (opt2 - 0x30);
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

	static char32_t bad_escape[5] = {0};
	bad_escape[0] = '\\';
	bad_escape[1] = escape;
	bad_escape[2] = opt1;
	bad_escape[3] = opt2;
	*error = bad_escape;

	return '\0';
}
