#include <stdbool.h>
#include <stdio.h>

typedef bool (*test_t)(void);
typedef bool (*tests_t[])(void);

void run_single_test(test_t test, bool *failed) {
	if (test()) {
		printf("F");
		*failed=true;
	}
	else {
		printf(".");
	}
}

void run_many_tests(const char *name, tests_t tests, bool *failed) {
	printf("%s ", name);

	unsigned int i=0;
	while (tests[i]!=NULL) {
		run_single_test(tests[i], failed);
		i++;
	}

	printf("\n");
}

bool dummy_function() {
	return false;
}

void testing_test_self(bool *failed) {
	tests_t tests={
		dummy_function,
		dummy_function,
		dummy_function,
		NULL
	};

	run_many_tests(__FILE__, tests, failed);
}
