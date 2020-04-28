#pragma once

#include <stdbool.h>
#include <stdio.h>

typedef bool (*test_t)(void);
typedef bool (*tests_t[])(void);

void run_single_test(test_t test, bool *pass) {
	if (!test()) {
		printf("F");
		*pass=false;
	}
	else {
		printf(".");
	}
}

void run_many_tests(const char *name, tests_t tests, bool *pass) {
	printf("%s ", name);

	unsigned int i=0;
	while (tests[i]!=NULL) {
		run_single_test(tests[i], pass);
		i++;
	}

	printf("\n");
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
