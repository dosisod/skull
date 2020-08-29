#include <stdbool.h>
#include <string.h>
#include <uchar.h>

#include "skull/common/str.h"
#include "skull/errors.h"

#include "test/testing.h"

TEST(strrstr, {
	const char *str1="aaaaa";
	const char *str2="ababab";
	const char *str3="abcdefg";

	return (
		strrstr(str1, "a")==(str1 + 4) &&
		!strrstr(str1, "b") &&
		strrstr(str2, "ab")==(str2 + 4) &&
		strrstr(str3, "abc")==str3
	);
})

TEST(c32sncpy, {
	const char32_t *data=U"abc";
	char32_t buf[4];

	c32sncpy(buf, data, 4);

	return (
		c32slen(buf)==3 &&
		c32scmp(buf, U"abc")
	);
})

TEST(c32sncpy_fill_nulls, {
	const char32_t *data=U"abc";
	char32_t buf[5];
	memset(buf, 0xff, 5 * sizeof *buf);

	c32sncpy(buf, data, 5);

	return (
		c32slen(buf)==3 &&
		c32scmp(buf, U"abc") &&
		buf[3]=='\0' &&
		buf[4]=='\0'
	);
})

TEST(c32slcpy, {
	const char32_t *data=U"hello there!";
	char32_t buf[6];

	c32slcpy(buf, data, 6);

	return (
		c32slen(buf)==5 &&
		c32scmp(buf, U"hello")
	);
})

TEST(c32sdup, {
	char32_t *str=c32sdup(U"hello world");

	const bool pass=c32scmp(U"hello world", str);

	free(str);
	return pass;
})

TEST(c32scat, {
	char32_t *str=c32scat(U"hello", U" world!");

	const bool pass=c32scmp(str, U"hello world!");

	free(str);
	return pass;
})

TEST(c32slen, {
	return (
		c32slen(U"abc")==3 &&
		c32slen(U"")==0 &&
		c32slen(NULL)==0
	);
})

TEST(c32stombs, {
	char *str=c32stombs(U"hello world! 存");

	const bool pass=strcmp(str, "hello world! 存")==0;

	free(str);
	return pass;
})

TEST(c32stombs_with_null, {
	return !c32stombs(NULL);
})

TEST(mbstoc32s, {
	char32_t *str=mbstoc32s("left 字 right");

	const bool pass=c32scmp(
		str,
		U"left 字 right"
	);

	free(str);
	return pass;
})

TEST(c32scmp, {
	return (
		c32scmp(U"abc", U"abc") &&
		!c32scmp(U"abc", U"abcdef") &&
		!c32scmp(U"abcdef", U"abc") &&
		!c32scmp(U"x", U"y")
	);
})

TEST(c32sncmp, {
	return (
		c32sncmp(U"abc", U"abc", 3) &&
		c32sncmp(U"abc", U"abcdef", 3) &&
		c32sncmp(U"abcdef", U"abc", 3) &&
		c32sncmp(U"ax", U"ay", 1) &&
		!c32sncmp(U"x", U"y", 1) &&
		!c32sncmp(U"aaa", U"a", 3)
	);
})

TEST(c32schr, {
	const char32_t *str=U"abcdefg";

	return (
		c32schr(str, 'a')==str &&
		c32schr(str, 'b')==(str + 1) &&
		c32schr(str, 'c')==(str + 2) &&
		!c32schr(str, 'x')
	);
})

TEST(c32isdigit, {
	return (
		!c32isdigit('/') &&
		c32isdigit('0') &&
		c32isdigit('9') &&
		!c32isdigit(':')
	);
})

TEST(c32isalnum, {
	return (
		!c32isalnum('/') &&
		c32isalnum('0') &&
		c32isalnum('9') &&
		!c32isalnum(':') &&
		!c32isalnum('@') &&
		c32isalnum('A') &&
		c32isalnum('Z') &&
		!c32isalnum('[') &&
		!c32isalnum('`') &&
		c32isalnum('a') &&
		c32isalnum('z') &&
		!c32isalnum('{')
	);
})

TEST(c32isxdigit, {
	return (
		!c32isxdigit('/') &&
		c32isxdigit('0') &&
		c32isxdigit('9') &&
		!c32isxdigit(':') &&
		!c32isxdigit('@') &&
		c32isxdigit('A') &&
		c32isxdigit('F') &&
		!c32isxdigit('G') &&
		!c32isxdigit('`') &&
		c32isxdigit('a') &&
		c32isxdigit('f') &&
		!c32isxdigit('g')
	);
})

bool c32sunescape_wrapper(const char32_t *str, const char32_t expected, const char32_t *expected_err) {
	const char32_t *err = NULL;
	char32_t c = c32sunescape(str, &err);

	return (
		expected == c &&
		expected_err ?
			c32scmp(expected_err, err) :
			expected_err == err
	);
}

TEST(c32sunescape, {
	return (
		c32sunescape_wrapper(U"\\\\", '\\', NULL) &&
		c32sunescape_wrapper(U"\\r", '\r', NULL) &&
		c32sunescape_wrapper(U"\\n", '\n', NULL) &&
		c32sunescape_wrapper(U"\\t", '\t', NULL) &&
		c32sunescape_wrapper(U"\\", '\0', _ERR_BAD_ESCAPE(U"\\"))
	);
})

void str_test_self(bool *pass) {
	tests_t tests={
		test_strrstr,
		test_c32sncpy,
		test_c32sncpy_fill_nulls,
		test_c32slcpy,
		test_c32sdup,
		test_c32scat,
		test_c32slen,
		test_c32stombs,
		test_c32stombs_with_null,
		test_mbstoc32s,
		test_c32scmp,
		test_c32sncmp,
		test_c32schr,
		test_c32isalnum,
		test_c32isdigit,
		test_c32isxdigit,
		test_c32sunescape,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
