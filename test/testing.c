#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../skull/common/color.h"
#include "skull/common/malloc.h"

#include "test/testing.h"

static Fail *fails_head = NULL;
static Fail *fails_last = NULL;

void run_single_test(Test test, bool *pass) {
	const char *name = NULL;
	if (!test(&name)) {
		printf(COLOR_BOLD COLOR_RED_FG "F" COLOR_RESET);
		*pass = false;

		Fail *fail;
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

void run_many_tests(const char *name, Test tests[], bool *pass) {
	printf("%s ", name);

	while(*tests) {
		run_single_test(*tests, pass);
		tests++;
	}

	putchar('\n');

	if (fails_head) {
		Fail *current = fails_head;
		Fail *tmp = NULL;

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

TEST(pass_macro, {
	PASS;
})

bool test_fail_macro_(void) {
	FAIL;
}

TEST(fail_macro, {
	return test_fail_macro_() == false;
})

TEST(assert_truthy, {
	ASSERT_TRUTHY(true);
})

TEST(assert_falsey, {
	ASSERT_FALSEY(false);
})

TEST(assert_equal, {
	ASSERT_EQUAL(1, 1);
})

TEST(assert_not_equal, {
	ASSERT_NOT_EQUAL(1, 2);
})

TEST(empty_test_passes, {})

TEST_SELF(testing,
	test_pass_macro,
	test_fail_macro,
	test_assert_truthy,
	test_assert_falsey,
	test_assert_equal,
	test_assert_not_equal,
	test_empty_test_passes
)
