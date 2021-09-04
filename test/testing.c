#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "../skull/common/color.h"
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
