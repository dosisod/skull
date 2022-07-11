#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>

#include "skull/common/errors.h"
#include "skull/common/str.h"

#include "test/skull/common/str.h"
#include "test/testing.h"

static bool test_strrstr(void) {
	const char *str1 = "aaaaa";
	const char *str2 = "ababab";
	const char *str3 = "abcdefg";

	ASSERT_FALSEY(strrstr(str1, "b"));

	ASSERT_EQUAL(strrstr(str1, "a"), str1 + 4);
	ASSERT_EQUAL(strrstr(str2, "ab"), str2 + 4);
	ASSERT_EQUAL(strrstr(str3, "abc"), str3);

	PASS
}

static bool test_c32sncpy(void) {
	const char32_t *data = U"abc";
	char32_t buf[4];

	c32sncpy(buf, data, 4);

	ASSERT_EQUAL(c32slen(buf), 3);
	ASSERT_TRUTHY(c32scmp(buf, U"abc"));

	PASS
}

static bool test_c32sncpy_fill_nulls(void) {
	const char32_t *data = U"abc";
	char32_t buf[5];
	memset(buf, 0xff, 5 * sizeof *buf);

	c32sncpy(buf, data, 5);

	ASSERT_TRUTHY(c32scmp(buf, U"abc"));
	ASSERT_EQUAL(c32slen(buf), 3);
	ASSERT_EQUAL(buf[3], '\0');
	ASSERT_EQUAL(buf[4], '\0');

	PASS
}

static bool test_c32sdup(void) {
	char32_t *str = c32sdup(U"hello world");

	ASSERT_TRUTHY(c32scmp(str, U"hello world"));

	free(str);

	PASS
}

static bool test_c32slen(void) {
	ASSERT_EQUAL(c32slen(U"abc"), 3);
	ASSERT_EQUAL(c32slen(U""), 0);
	ASSERT_EQUAL(c32slen(NULL), 0);

	PASS
}

static bool test_c32stombs(void) {
	char *str = c32stombs(U"hello world! 存", NULL);

	ASSERT_EQUAL(strcmp(str, "hello world! 存"), 0);

	free(str);

	PASS
}

static bool test_mbstoc32s(void) {
	char32_t *str = mbstoc32s("left 字 right");

	ASSERT_TRUTHY(c32scmp(str, U"left 字 right"));

	free(str);

	PASS
}

static bool test_c32scmp(void) {
	ASSERT_TRUTHY(c32scmp(U"abc", U"abc"));
	ASSERT_FALSEY(c32scmp(U"abc", U"abcdef"));
	ASSERT_FALSEY(c32scmp(U"abcdef", U"abc"));
	ASSERT_FALSEY(c32scmp(U"x", U"y"));

	PASS
}

static bool test_c32sncmp(void) {
	ASSERT_TRUTHY(c32sncmp(U"abc", U"abc", 3));
	ASSERT_TRUTHY(c32sncmp(U"abc", U"abcdef", 3));
	ASSERT_TRUTHY(c32sncmp(U"abcdef", U"abc", 3));
	ASSERT_TRUTHY(c32sncmp(U"ax", U"ay", 1));
	ASSERT_FALSEY(c32sncmp(U"x", U"y", 1));
	ASSERT_FALSEY(c32sncmp(U"aaa", U"a", 3));

	PASS
}

static bool test_c32isdigit(void) {
	ASSERT_FALSEY(c32isdigit('/'));
	ASSERT_TRUTHY(c32isdigit('0'));
	ASSERT_TRUTHY(c32isdigit('9'));
	ASSERT_FALSEY(c32isdigit(':'));

	PASS
}

static bool test_c32isalnum(void) {
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

	PASS
}

static bool test_c32isxdigit(void) {
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

	PASS
}

static bool test_uvsnsprintf(void) {
	char *str = uvsnprintf("%s %s %i!", "hello", "world", 1234);

	ASSERT_EQUAL(strcmp(str, "hello world 1234!"), 0);

	free(str);

	PASS
}

static bool c32sunescape_fixture(
	const char32_t *str,
	const char32_t char_expected,
	unsigned offset,
	const char32_t *err_expected
) {
	const char32_t *err = NULL;
	const char32_t *copy = str;

	const char32_t c = c32sunescape(&str, &err);

	ASSERT_EQUAL(c, char_expected);
	ASSERT_EQUAL(copy + offset, str);
	ASSERT_TRUTHY(c32scmp(
		err ? err : U"",
		err_expected ? err_expected : U""
	));

	PASS
}

static bool test_c32sunescape(void) {
	ASSERT_TRUTHY(c32sunescape_fixture(U"\\e", '\033', 1, NULL));
	ASSERT_TRUTHY(c32sunescape_fixture(U"x", 'x', 0, NULL));

	ASSERT_TRUTHY(c32sunescape_fixture(U"\\\'", '\'', 1, NULL));
	ASSERT_TRUTHY(c32sunescape_fixture(U"\\\"", '\"', 1, NULL));
	ASSERT_TRUTHY(c32sunescape_fixture(U"\\\\", '\\', 1, NULL));

	ASSERT_TRUTHY(c32sunescape_fixture(U"\\x", '\0', 0, U"\\x"));
	ASSERT_TRUTHY(c32sunescape_fixture(U"\\xz", '\0', 0, U"\\xz"));

	ASSERT_TRUTHY(c32sunescape_fixture(U"\\x41", U'\x41', 3, NULL));
	ASSERT_TRUTHY(c32sunescape_fixture(U"\\xFF", U'\xFF', 3, NULL));
	ASSERT_TRUTHY(c32sunescape_fixture(U"\\xFFF", U'\xFFF', 4, NULL));
	ASSERT_TRUTHY(c32sunescape_fixture(U"\\xFFFF", U'\xFFFF', 5, NULL));
	ASSERT_TRUTHY(c32sunescape_fixture(U"\\x9", '\x09', 2, NULL));
	ASSERT_TRUTHY(c32sunescape_fixture(U"\\xdhello", '\x0d', 2, NULL));
	ASSERT_TRUTHY(c32sunescape_fixture(U"\\x1F480", U'\x1F480', 6, NULL));

	ASSERT_TRUTHY(c32sunescape_fixture(U"\\xFFFFFFFF\'", 0xFFFFFFFF, 9, NULL))
	ASSERT_TRUTHY(c32sunescape_fixture(U"\\xFFFFFFFFF", 0xFFFFFFFF, 9, NULL))

	PASS
}

static bool test_mbstoc32s_illegal_utf8(void) {
	ASSERT_FALSEY(mbstoc32s("\xc3\x28\n"));

	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: illegal UTF8 sequence at character offset 0\n"
	));

	PASS
}

static bool test_c32schr(void) {
	const char32_t *str = U"abc";

	ASSERT_FALSEY(c32schr(str, 'x'));
	ASSERT_EQUAL(c32schr(str, 'a'), str);
	ASSERT_EQUAL(c32schr(str, 'c'), str + 2);

	PASS
}

void str_test_self(bool *pass) {
	RUN_ALL(
		test_strrstr,
		test_c32sncpy,
		test_c32sncpy_fill_nulls,
		test_c32sdup,
		test_c32slen,
		test_c32stombs,
		test_mbstoc32s,
		test_c32scmp,
		test_c32sncmp,
		test_c32isalnum,
		test_c32isdigit,
		test_c32isxdigit,
		test_uvsnsprintf,
		test_c32sunescape,
		test_mbstoc32s_illegal_utf8,
		test_c32schr
	)
}
