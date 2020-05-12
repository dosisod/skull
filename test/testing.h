#pragma once

#include <stdbool.h>
#include <wchar.h>

typedef bool (*test_t)(void);
typedef bool (*tests_t[])(void);

void run_single_test(test_t test, bool *pass) {
	if (!test()) {
		wprintf(L"F");
		*pass=false;
	}
	else {
		wprintf(L".");
	}
}

void run_many_tests(const char *name, tests_t tests, bool *pass) {
	wprintf(L"%s ", name);

	while(*tests!=NULL) {
		run_single_test(*tests, pass);
		tests++;
	}

	wprintf(L"\n");
}

bool dummy_function() {
	return true;
}

void testing_test_self(bool *pass) {
	tests_t tests={
		dummy_function,
		dummy_function,
		dummy_function,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
