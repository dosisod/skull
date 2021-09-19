#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "../skull/common/color.h"
#include "../skull/common/errors.h"
#include "../skull/common/range.h"
#include "../skull/common/vector.h"
#include "skull/common/malloc.h"

#include "test/testing.h"

void run_many_tests(Test tests[], bool *pass) {
	while(*tests) {
		if (!(*tests)()) {
			*pass = false;
		}
		tests++;
	}
}

bool __attribute__((const)) test_pass_macro() {
	PASS;
}

bool test_assert_truthy() {
	ASSERT_TRUTHY(true);

	PASS;
}

bool test_assert_falsey() {
	ASSERT_FALSEY(false);

	PASS;
}

bool test_assert_equal() {
	ASSERT_EQUAL(1, 1);

	PASS;
}

void testing_test_self(bool *pass) {
	RUN_ALL(
		test_pass_macro,
		test_assert_truthy,
		test_assert_falsey,
		test_assert_equal
	)
}

bool compare_errors(const char *expected) {
	if (!error_msgs || error_msgs->length == 0) {
		return true;
	}

	bool fail = false;

	const char *current_line = expected;
	const char *newline;

	for (size_t i = 0 ; i < error_msgs->length ; i++) {
		newline = strchr(current_line, '\n');

		if (!newline || strncmp(
			current_line,
			error_msgs->elements[i],
			(size_t)(newline - current_line)) != 0
		) {
			fail = true;
			break;
		}

		current_line = newline + 1;
	}

	if (strchr(current_line, '\n')) fail = true;

	free_vector(error_msgs, free);
	error_msgs = NULL;

	return fail;
}
