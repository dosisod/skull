#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>

#include "skull/common/errors.h"
#include "skull/common/str.h"

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
})

TEST(c32sncpy, {
	const char32_t *data = U"abc";
	char32_t buf[4];

	c32sncpy(buf, data, 4);

	ASSERT_EQUAL(c32slen(buf), 3);
	ASSERT_TRUTHY(c32scmp(buf, U"abc"));
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
})

TEST(c32sdup, {
	char32_t *str = c32sdup(U"hello world");

	ASSERT_TRUTHY(c32scmp(str, U"hello world"));

	free(str);
})

TEST(c32slen, {
	ASSERT_EQUAL(c32slen(U"abc"), 3);
	ASSERT_EQUAL(c32slen(U""), 0);
	ASSERT_EQUAL(c32slen(NULL), 0);
})

TEST(c32stombs, {
	char *str = c32stombs(U"hello world! 存", NULL);

	ASSERT_EQUAL(strcmp(str, "hello world! 存"), 0);

	free(str);
})

TEST(c32stombs_with_null, {
	ASSERT_FALSEY(c32stombs(NULL, NULL));
})

TEST(mbstoc32s, {
	char32_t *str = mbstoc32s("left 字 right");

	ASSERT_TRUTHY(c32scmp(str, U"left 字 right"));

	free(str);
})

TEST(c32scmp, {
	ASSERT_TRUTHY(c32scmp(U"abc", U"abc"));
	ASSERT_FALSEY(c32scmp(U"abc", U"abcdef"));
	ASSERT_FALSEY(c32scmp(U"abcdef", U"abc"));
	ASSERT_FALSEY(c32scmp(U"x", U"y"));
})

TEST(c32sncmp, {
	ASSERT_TRUTHY(c32sncmp(U"abc", U"abc", 3));
	ASSERT_TRUTHY(c32sncmp(U"abc", U"abcdef", 3));
	ASSERT_TRUTHY(c32sncmp(U"abcdef", U"abc", 3));
	ASSERT_TRUTHY(c32sncmp(U"ax", U"ay", 1));
	ASSERT_FALSEY(c32sncmp(U"x", U"y", 1));
	ASSERT_FALSEY(c32sncmp(U"aaa", U"a", 3));
})

TEST(c32isdigit, {
	ASSERT_FALSEY(c32isdigit('/'));
	ASSERT_TRUTHY(c32isdigit('0'));
	ASSERT_TRUTHY(c32isdigit('9'));
	ASSERT_FALSEY(c32isdigit(':'));
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
})

TEST(uvsnsprintf, {
	char *str = uvsnprintf("%s %s %i!", "hello", "world", 1234);

	ASSERT_EQUAL(strcmp(str, "hello world 1234!"), 0);

	free(str);
})

TEST_SELF(str,
	test_strrstr,
	test_c32sncpy,
	test_c32sncpy_fill_nulls,
	test_c32sdup,
	test_c32slen,
	test_c32stombs,
	test_c32stombs_with_null,
	test_mbstoc32s,
	test_c32scmp,
	test_c32sncmp,
	test_c32isalnum,
	test_c32isdigit,
	test_c32isxdigit,
	test_uvsnsprintf
)
