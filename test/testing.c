#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "../skull/common/color.h"
#include "../skull/common/range.h"
#include "../skull/common/vector.h"
#include "skull/common/malloc.h"

#include "test/testing.h"

static Vector *fails;

MAKE_VECTOR(Fail)

const char *FUNC_NAME = NULL;

void run_single_test(Test test, bool *pass) {
	const char *msg = NULL;

	if (!test(&msg)) {
		printf(COLOR_BOLD COLOR_RED_FG "F" COLOR_RESET);
		*pass = false;

		Fail *fail;
		fail = Malloc(sizeof *fail);

		fail->msg = msg;
		fail->name = FUNC_NAME;

		vector_push_Fail(fails, fail);
	}
	else {
		printf(COLOR_BOLD COLOR_GREEN_FG "." COLOR_RESET);
	}
}

void run_many_tests(const char *name, Test tests[], bool *pass) {
	printf("%s ", name);

	fails = make_vector();

	while(*tests) {
		run_single_test(*tests, pass);
		tests++;
	}

	putchar('\n');

	if (fails->length) {
		for RANGE(i, fails->length) {
			Fail *current = fails->elements[i];

			printf("%s " COLOR_BOLD COLOR_RED_FG "FAILED\n" COLOR_RESET, current->name);
			printf("  %s\n\n", current->msg);

			free(current);
		}
	}

	free_vector(fails, free);
}

TEST(pass_macro, {
	PASS;
})

__attribute__((const)) bool test_fail_macro_(void) {
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
