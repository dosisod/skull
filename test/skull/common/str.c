#include <stdbool.h>
#include <string.h>
#include <uchar.h>

#include "skull/common/str.h"
#include "skull/errors.h"

#include "test/skull/common/str.h"
#include "test/testing.h"

TEST(strrstr, {
	const char *str1 = "aaaaa";
	const char *str2 = "ababab";
	const char *str3 = "abcdefg";

	ASSERT_FALSEY(strrstr(str1, "b"));

	ASSERT_EQUAL(strrstr(str1, "a"), str1 + 4);
	ASSERT_EQUAL(strrstr(str2, "ab"), str2 + 4);
	ASSERT_EQUAL(strrstr(str3, "abc"), str3);

	PASS;
})

TEST(c32sncpy, {
	const char32_t *data = U"abc";
	char32_t buf[4];

	c32sncpy(buf, data, 4);

	ASSERT_EQUAL(c32slen(buf), 3);
	ASSERT_TRUTHY(c32scmp(buf, U"abc"));

	PASS;
})

TEST(c32sncpy_fill_nulls, {
	const char32_t *data = U"abc";
	char32_t buf[5];
	memset(buf, 0xff, 5 * sizeof *buf);

	c32sncpy(buf, data, 5);

	ASSERT_TRUTHY(c32scmp(buf, U"abc"));
	ASSERT_EQUAL(c32slen(buf), 3);
	ASSERT_EQUAL(buf[3], '\0');
	ASSERT_EQUAL(buf[4], '\0');

	PASS;
})

TEST(c32slcpy, {
	const char32_t *data = U"hello there!";
	char32_t buf[6];

	c32slcpy(buf, data, 6);

	ASSERT_EQUAL(c32slen(buf), 5);
	ASSERT_TRUTHY(c32scmp(buf, U"hello"));

	PASS;
})

TEST(c32sdup, {
	char32_t *str = c32sdup(U"hello world");

	ASSERT_TRUTHY(c32scmp(str, U"hello world"));

	free(str);
	PASS;
})

TEST(c32scat, {
	char32_t *str = c32scat(U"hello", U" world!");

	ASSERT_TRUTHY(c32scmp(str, U"hello world!"));

	free(str);
	PASS;
})

TEST(c32slen, {
	ASSERT_EQUAL(c32slen(U"abc"), 3);
	ASSERT_EQUAL(c32slen(U""), 0);
	ASSERT_EQUAL(c32slen(NULL), 0);

	PASS;
})

TEST(c32stombs, {
	char *str = c32stombs(U"hello world! 存");

	ASSERT_EQUAL(strcmp(str, "hello world! 存"), 0);

	free(str);
	PASS;
})

TEST(c32stombs_with_null, {
	ASSERT_FALSEY(c32stombs(NULL));

	PASS;
})

TEST(mbstoc32s, {
	char32_t *str = mbstoc32s("left 字 right");

	ASSERT_TRUTHY(c32scmp(str, U"left 字 right"));

	free(str);
	PASS;
})

TEST(c32scmp, {
	ASSERT_TRUTHY(c32scmp(U"abc", U"abc"));
	ASSERT_FALSEY(c32scmp(U"abc", U"abcdef"));
	ASSERT_FALSEY(c32scmp(U"abcdef", U"abc"));
	ASSERT_FALSEY(c32scmp(U"x", U"y"));

	PASS;
})

TEST(c32sncmp, {
	ASSERT_TRUTHY(c32sncmp(U"abc", U"abc", 3));
	ASSERT_TRUTHY(c32sncmp(U"abc", U"abcdef", 3));
	ASSERT_TRUTHY(c32sncmp(U"abcdef", U"abc", 3));
	ASSERT_TRUTHY(c32sncmp(U"ax", U"ay", 1));
	ASSERT_FALSEY(c32sncmp(U"x", U"y", 1));
	ASSERT_FALSEY(c32sncmp(U"aaa", U"a", 3));

	PASS;
})

TEST(c32schr, {
	const char32_t *str = U"abcdefg";

	ASSERT_EQUAL(c32schr(str, 'a'), str);
	ASSERT_EQUAL(c32schr(str, 'b'), str + 1);
	ASSERT_EQUAL(c32schr(str, 'c'), str + 2);
	ASSERT_FALSEY(c32schr(str, 'x'));

	PASS;
})

TEST(c32isdigit, {
	ASSERT_FALSEY(c32isdigit('/'));
	ASSERT_TRUTHY(c32isdigit('0'));
	ASSERT_TRUTHY(c32isdigit('9'));
	ASSERT_FALSEY(c32isdigit(':'));

	PASS;
})

TEST(c32isalnum, {
	ASSERT_FALSEY(c32isalnum('/'));
	ASSERT_TRUTHY(c32isalnum('0'));
	ASSERT_TRUTHY(c32isalnum('9'));
	ASSERT_FALSEY(c32isalnum(':'));
	ASSERT_FALSEY(c32isalnum('@'));
	ASSERT_TRUTHY(c32isalnum('A'));
	ASSERT_TRUTHY(c32isalnum('Z'));
	ASSERT_FALSEY(c32isalnum('['));
	ASSERT_FALSEY(c32isalnum('`'));
	ASSERT_TRUTHY(c32isalnum('a'));
	ASSERT_TRUTHY(c32isalnum('z'));
	ASSERT_FALSEY(c32isalnum('{'));

	PASS;
})

TEST(c32isxdigit, {
	ASSERT_FALSEY(c32isxdigit('/'));
	ASSERT_TRUTHY(c32isxdigit('0'));
	ASSERT_TRUTHY(c32isxdigit('9'));
	ASSERT_FALSEY(c32isxdigit(':'));
	ASSERT_FALSEY(c32isxdigit('@'));
	ASSERT_TRUTHY(c32isxdigit('A'));
	ASSERT_TRUTHY(c32isxdigit('F'));
	ASSERT_FALSEY(c32isxdigit('G'));
	ASSERT_FALSEY(c32isxdigit('`'));
	ASSERT_TRUTHY(c32isxdigit('a'));
	ASSERT_TRUTHY(c32isxdigit('f'));
	ASSERT_FALSEY(c32isxdigit('g'));

	PASS;
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
	ASSERT_TRUTHY(c32sunescape_wrapper(U"\\\\", '\\', NULL));
	ASSERT_TRUTHY(c32sunescape_wrapper(U"\\r", '\r', NULL));
	ASSERT_TRUTHY(c32sunescape_wrapper(U"\\n", '\n', NULL));
	ASSERT_TRUTHY(c32sunescape_wrapper(U"\\t", '\t', NULL));
	ASSERT_TRUTHY(c32sunescape_wrapper(U"\\", '\0', ERR_BAD_ESCAPE_(U"\\")));

	PASS;
})

TEST_SELF(str,
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
	test_c32sunescape
)
