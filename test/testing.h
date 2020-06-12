#pragma once

#include <stdbool.h>
#include <wchar.h>

#include "../src/common/color.h"

typedef bool (*test_t)(const char** func);
typedef bool (*tests_t[])(const char** func);

#define TEST(name, code) \
bool test_##name (const char** func) {*func=__func__;(code);}

typedef struct fail_t {
	const char *name;
	struct fail_t *next;
} fail_t;

fail_t *fails_head=NULL;
fail_t *fails_last=NULL;

void run_single_test(test_t test, bool *pass) {
	const char *name=NULL;
	if (!test(&name)) {
		wprintf(COLOR_BOLD COLOR_RED_FG L"F" COLOR_RESET);
		*pass=false;

		fail_t *fail=malloc(sizeof(fail_t));
		fail->next=NULL;

		if (fails_head==NULL) {
			fail->next=NULL;
			fails_head=fail;
			fails_last=fail;
		}
		else {
			fails_last->next=fail;
			fails_last=fail;
		}
		fails_last->name=name;
	}
	else {
		wprintf(COLOR_BOLD COLOR_GREEN_FG L"." COLOR_RESET);
	}
}

void run_many_tests(const char *name, tests_t tests, bool *pass) {
	wprintf(L"%s ", name);

	while(*tests!=NULL) {
		run_single_test(*tests, pass);
		tests++;
	}

	wprintf(L"\n");

	if (fails_head!=NULL) {
		fail_t *current=fails_head;
		fail_t *tmp=NULL;

		while (current!=NULL) {
			wprintf(L"%s " COLOR_BOLD COLOR_RED_FG "FAILED\n" COLOR_RESET, current->name);

			tmp=current;
			current=current->next;
			free(tmp);
		}
		wprintf(L"\n");

		fails_last=NULL;
		fails_head=NULL;
	}
}

TEST(dummy_function, {
	return true;
})

void testing_test_self(bool *pass) {
	tests_t tests={
		test_dummy_function,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
