#pragma once

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef _Bool (*Test)(const char** func);

#define TEST(name, code) \
_Bool test_##name (const char **func) { \
	*func = __func__; \
	code; \
	return 1; \
}

#define TEST_DECL(name) \
_Bool test_##name (const char **func);

#define TEST_SELF(name, ...) \
void name##_test_self (_Bool *pass) { \
	run_many_tests(__FILE__, (Test[]){ \
		__VA_ARGS__, NULL \
	}, pass); \
}

#define PASS return 1

#define FAIL return 0

#define ASSERT_TRUTHY(x) \
	if (!(x)) { return 0; }

#define ASSERT_FALSEY(x) \
	if ((x)) { return 0; }

#define ASSERT_NOT_EQUAL(x, y) \
	if ((x) == (y)) { return 0; }

#define ASSERT_EQUAL(x, y) \
	if ((x) != (y)) { return 0; }

typedef struct Fail {
	const char *name;
	struct Fail *next;
} Fail;

void run_single_test(Test, _Bool *);

void run_many_tests(const char *, Test [], _Bool *);

void testing_test_self(_Bool *);
