#include <stdbool.h>
#include <uchar.h>

#include "../../src/common/str.h"
#include "../../test/testing.h"

TEST(strlcpy, {
	const char *data="hello there!";
	const size_t len=5;
	char buf[len + 1];

	strlcpy(buf, data, len + 1);

	return (
		strlen(buf)==len &&
		strcmp(buf, "hello")==0
	);
})

TEST(c32sncpy, {
	const char32_t *data=U"abc";
	char32_t buf[4];

	c32sncpy(buf, data, 3);
	buf[3]=U'\0';

	return (
		c32slen(buf)==3 &&
		c32scmp(buf, U"abc")
	);
})

TEST(wcslcpy, {
	const wchar_t *data=L"hello there!";
	const size_t len=5;
	wchar_t buf[len + 1];

	wcslcpy(buf, data, len + 1);

	return (
		wcslen(buf)==len &&
		wcscmp(buf, L"hello")==0
	);
})

TEST(c32slcpy, {
	const char32_t *data=U"hello there!";
	const size_t len=5;
	char32_t buf[len + 1];

	c32slcpy(buf, data, len + 1);

	return (
		c32slen(buf)==len &&
		c32scmp(buf, U"hello")
	);
})

TEST(c32sdup, {
	char32_t *str=c32sdup(U"hello world");

	const bool pass=c32scmp(U"hello world", str);

	free(str);
	return pass;
})

TEST(wcsdup, {
	wchar_t *str=wcsdup(L"hello world");

	const bool pass=wcscmp(L"hello world", str)==0;

	free(str);
	return pass;
})

TEST(c32slen, {
	return (
		c32slen(U"abc")==3 &&
		c32slen(U"")==0
	);
})

TEST(c32stombs, {
	char *str=c32stombs(U"hello world! 存");

	const bool pass=strcmp(str, "hello world! 存")==0;

	free(str);
	return pass;
})

TEST(mbstoc32s, {
	char32_t *str=mbstoc32s("hello world! 存");
	char32_t *copy=str;

	const char32_t *cmp=U"hello world! 存";

	bool pass=true;
	while (*str!=U'\0' && *cmp!=U'\0') {
		if (*str!=*cmp) {
			break;
		}
		str++;
		cmp++;
	}
	if (pass && (*str!=U'\0' || *cmp!=U'\0')) {
		pass=false;
	}

	free(copy);
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
		!c32sncmp(U"x", U"y", 1)
	);
})

TEST(c32schr, {
	const char32_t *str=U"abcdefg";

	return (
		c32schr(str, U'a')==str &&
		c32schr(str, U'b')==(str + 1) &&
		c32schr(str, U'c')==(str + 2) &&
		c32schr(str, U'x')==NULL
	);
})

TEST(c32isdigit, {
	return (
		!c32isdigit(U'/') &&
		c32isdigit(U'0') &&
		c32isdigit(U'9') &&
		!c32isdigit(U':')
	);
})

TEST(c32isalnum, {
	return (
		!c32isalnum(U'/') &&
		c32isalnum(U'0') &&
		c32isalnum(U'9') &&
		!c32isalnum(U':') &&
		!c32isalnum(U'@') &&
		c32isalnum(U'A') &&
		c32isalnum(U'Z') &&
		!c32isalnum(U'[') &&
		!c32isalnum(U'`') &&
		c32isalnum(U'a') &&
		c32isalnum(U'z') &&
		!c32isalnum(U'{')
	);
})

TEST(c32isxdigit, {
	return (
		!c32isxdigit(U'/') &&
		c32isxdigit(U'0') &&
		c32isxdigit(U'9') &&
		!c32isxdigit(U':') &&
		!c32isxdigit(U'@') &&
		c32isxdigit(U'A') &&
		c32isxdigit(U'F') &&
		!c32isxdigit(U'G') &&
		!c32isxdigit(U'`') &&
		c32isxdigit(U'a') &&
		c32isxdigit(U'f') &&
		!c32isxdigit(U'g')
	);
})

void str_test_self(bool *pass) {
	tests_t tests={
		test_strlcpy,
		test_c32sncpy,
		test_wcslcpy,
		test_c32slcpy,
		test_wcsdup,
		test_c32sdup,
		test_c32slen,
		test_c32stombs,
		test_mbstoc32s,
		test_c32scmp,
		test_c32sncmp,
		test_c32schr,
		test_c32isalnum,
		test_c32isdigit,
		test_c32isxdigit,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
