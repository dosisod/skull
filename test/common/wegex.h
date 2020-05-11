
#include <stdbool.h>

#include "../src/parse/tokenize.h"
#include "../test/testing.h"

bool test_no_modifiers() {
	return (
		wegex_match(L"abc", L"abc") &&
		!wegex_match(L"abc", L"abcd") &&
		!wegex_match(L"abc", L"a")
	);
}

bool test_star_modifier() {
	return (
		wegex_match(L"*x", L"x") &&
		wegex_match(L"*x", L"xxx") &&
		wegex_match(L"*xa", L"a") &&
		wegex_match(L"*xa", L"xxxxa") &&
		!wegex_match(L"*x", L"xa") &&
		!wegex_match(L"*x", L"")
	);
}

bool test_plus_modifier() {
	return (
		wegex_match(L"+x", L"x") &&
		wegex_match(L"+x", L"xxx") &&
		!wegex_match(L"+xa", L"a") &&
		!wegex_match(L"+x", L"xa") &&
		!wegex_match(L"+x", L"")
	);
}

bool test_ternary_modifier() {
	return (
		wegex_match(L"?x", L"x") &&
		wegex_match(L"?xa", L"xa") &&
		wegex_match(L"?xa", L"a") &&
		!wegex_match(L"?x", L"")
	);
}

void wegex_test_self(bool *pass) {
	tests_t tests={
		test_no_modifiers,
		test_star_modifier,
		test_plus_modifier,
		test_ternary_modifier,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
