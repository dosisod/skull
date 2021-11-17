#pragma once

#include <stddef.h>
#include <stdio.h>

#include "../skull/common/color.h"

typedef _Bool (*Test)(void);

#define RUN_ALL(...) \
	run_many_tests((Test[]){ \
		__VA_ARGS__, NULL \
	}, pass);

#define PASS return 1;

#define FAIL \
	printf(__FILE__":%s:%i: ", __func__, __LINE__); \
	puts(COLOR_BOLD COLOR_RED_FG "FAILED " COLOR_RESET); \
	return 0;

#define ASSERT_TRUTHY(x) \
	if (!(x)) { \
		printf(__FILE__":%s:%i: ", __func__, __LINE__); \
		printf( \
			COLOR_BOLD COLOR_RED_FG "FAILED " COLOR_RESET \
			"Expected `" #x "` to be truthy\n" \
		); \
		return 0; \
	}

#define ASSERT_FALSEY(x) \
	if ((x)) { \
		printf(__FILE__":%s:%i: ", __func__, __LINE__); \
		printf( \
			COLOR_BOLD COLOR_RED_FG "FAILED " COLOR_RESET \
			"Expected `" #x "` to be falsey\n" \
		); \
		return 0; \
	}

#define ASSERT_EQUAL(x, y) \
	if ((x) != (y)) { \
		printf(__FILE__":%s:%i: ", __func__, __LINE__); \
		printf( \
			COLOR_BOLD COLOR_RED_FG "FAILED " COLOR_RESET \
			"Expected `" #x "` and `" #y "` to be equal\n" \
		); \
		return 0; \
	}

void run_many_tests(Test [], _Bool *);

void testing_test_self(_Bool *);
_Bool compare_errors(const char *);
