#include <stdbool.h>

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

void str_test_self(bool *pass) {
	tests_t tests={
		test_strlcpy,
		test_wcslcpy,
		test_wcsdup,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
