#pragma once

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef _Bool (*test_t)(const char** func);
typedef _Bool (*tests_t[])(const char** func);

#define TEST(name, code) \
_Bool test_##name (const char **func) { \
	*func = __func__; \
	code; \
}

#define TEST_DECL(name) \
_Bool test_##name (const char **func);

typedef struct fail_t {
	const char *name;
	struct fail_t *next;
} fail_t;

fail_t *fails_head;
fail_t *fails_last;

void run_single_test(test_t test, _Bool *pass);

void run_many_tests(const char *name, tests_t tests, _Bool *pass);

void testing_test_self(_Bool *pass);
