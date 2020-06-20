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

void str_test_self(bool *pass) {
	tests_t tests={
		test_strlcpy,
		test_wcslcpy,
		test_wcsdup,
		test_c32slen,
		test_c32stombs,
		test_mbstoc32s,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
