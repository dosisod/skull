#pragma once

#include <stddef.h>
#include <stdio.h>

typedef _Bool (*Test)(const char **);

const char *FUNC_NAME;

#define TEST(name, code) \
_Bool test_##name (const char **msg) { \
	*msg = NULL; \
	FUNC_NAME = __func__; \
	code; \
	return 1; \
}

#define TEST_SELF(name, ...) \
void name##_test_self (_Bool *pass) { \
	run_many_tests(__FILE__, (Test[]){ \
		__VA_ARGS__, NULL \
	}, pass); \
}

#define PASS return 1

#define FAIL return 0

#define ASSERT_TRUTHY(x) \
	if (!(x)) { \
		*msg = "Expected `" #x "` to be truthy"; \
		return 0; \
	}

#define ASSERT_FALSEY(x) \
	if ((x)) { \
		*msg = "Expected `" #x "` to be falsey"; \
		return 0; \
	}

#define ASSERT_NOT_EQUAL(x, y) \
	if ((x) == (y)) { \
		*msg = "Expected `" #x "` and `" #y "` to be not be equal"; \
		return 0; \
	}

#define ASSERT_EQUAL(x, y) \
	if ((x) != (y)) { \
		*msg = "Expected `" #x "` and `" #y "` to be be equal"; \
		return 0; \
	}

typedef struct Fail {
	const char *name;
	const char *msg;
} Fail;

void run_single_test(Test, _Bool *);

void run_many_tests(const char *, Test [], _Bool *);

void testing_test_self(_Bool *);
