#pragma once

#include <stddef.h>
#include <stdio.h>

#include "../skull/common/color.h"

typedef _Bool (*Test)();

#define RUN_ALL(...) \
	run_many_tests((Test[]){ \
		__VA_ARGS__, NULL \
	}, pass);

#define PASS return 1;

#define ASSERT_TRUTHY(x) \
	printf(__FILE__":%s:%i: ", __func__, __LINE__); \
	if (!(x)) { \
		printf( \
			COLOR_BOLD COLOR_RED_FG "FAILED " COLOR_RESET \
			"Expected `" #x "` to be truthy\n" \
		); \
		return 0; \
	} \
	printf(COLOR_BOLD COLOR_GREEN_FG "PASSED" COLOR_RESET "\n");

#define ASSERT_FALSEY(x) \
	printf(__FILE__":%s:%i: ", __func__, __LINE__); \
	if ((x)) { \
		printf( \
			COLOR_BOLD COLOR_RED_FG "FAILED " COLOR_RESET \
			"Expected `" #x "` to be falsey\n" \
		); \
		return 0; \
	} \
	printf(COLOR_BOLD COLOR_GREEN_FG "PASSED" COLOR_RESET "\n");

#define ASSERT_EQUAL(x, y) \
	printf(__FILE__":%s:%i: ", __func__, __LINE__); \
	if ((x) != (y)) { \
		printf( \
			COLOR_BOLD COLOR_RED_FG "FAILED " COLOR_RESET \
			"Expected `" #x "` and `" #y "` to be equal\n" \
		); \
		return 0; \
	} \
	printf(COLOR_BOLD COLOR_GREEN_FG "PASSED" COLOR_RESET "\n");

void run_many_tests(Test [], _Bool *);

void testing_test_self(_Bool *);
