#include <stdbool.h>

#include "../src/common/str.h"
#include "../test/testing.h"

bool test_strncpyz() {
	const char *data="hello there!";
	int len=5;
	char buf[len + 1];

	strncpyz(buf, data, 5);

	return strlen(buf)==5;
}

bool test_samestr() {
	return samestr("a", "b")==(strcmp("a", "b")==0);
}

void str_test_self(bool *pass) {
	tests_t tests={
		test_strncpyz,
		test_samestr,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
