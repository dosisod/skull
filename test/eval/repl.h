#include <stdbool.h>

#include "../../src/eval/repl.h"
#include "../../test/testing.h"

bool test_repl_number_echo() {
	const wchar_t *output=repl_eval(L"0");

	return wcscmp(L"0", output)==0;
}

bool test_repl_variable_assign() {
	const wchar_t *output=repl_eval(L"x: int = 0");

	return output==NULL;
}

void repl_test_self(bool *pass) {
	tests_t tests={
		test_repl_number_echo,
		test_repl_variable_assign,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
