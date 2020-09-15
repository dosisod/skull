#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../skull/common/color.h"
#include "skull/common/malloc.h"

#include "test/testing.h"

fail_t *fails_head = NULL;
fail_t *fails_last = NULL;

void run_single_test(test_t test, bool *pass) {
	const char *name = NULL;
	if (!test(&name)) {
		printf(COLOR_BOLD COLOR_RED_FG "F" COLOR_RESET);
		*pass = false;

		fail_t *fail;
		fail = malloc(sizeof *fail);
		DIE_IF_MALLOC_FAILS(fail);

		fail->next = NULL;

		if (!fails_head) {
			fail->next = NULL;
			fails_head = fail;
			fails_last = fail;
		}
		else {
			fails_last->next = fail;
			fails_last = fail;
		}
		fails_last->name = name;
	}
	else {
		printf(COLOR_BOLD COLOR_GREEN_FG "." COLOR_RESET);
	}
}

void run_many_tests(const char *name, tests_t tests, bool *pass) {
	printf("%s ", name);

	while(*tests) {
		run_single_test(*tests, pass);
		tests++;
	}

	putchar('\n');

	if (fails_head) {
		fail_t *current = fails_head;
		fail_t *tmp = NULL;

		while (current) {
			printf("%s " COLOR_BOLD COLOR_RED_FG "FAILED\n" COLOR_RESET, current->name);

			tmp = current;
			current = current->next;
			free(tmp);
		}
		putchar('\n');

		fails_last = NULL;
		fails_head = NULL;
	}
}

TEST(dummy_function, {
	return true;
})

void testing_test_self(bool *pass) {
	tests_t tests = {
		test_dummy_function,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
