#include <stdbool.h>

#include "../../src/parse/tokenize.h"
#include "../../src/common/wegex.h"
#include "../../test/testing.h"

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
		wegex_match(L"*x", L"")
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
		wegex_match(L"?x", L"")
	);
}

bool test_charset_syntax() {
	return (
		wegex_match(L"+[abc]", L"a") &&
		wegex_match(L"+[abc]", L"b") &&
		wegex_match(L"+[abc]", L"c") &&
		wegex_match(L"+[abc]", L"aabbcc") &&
		!wegex_match(L"+[abc]", L"d") &&
		!wegex_match(L"+[abc]", L"ax")
	);
}

bool test_wegex_number_match() {
	return (
		wegex_match(L"\n", L"0") &&
		wegex_match(L"\n", L"1") &&
		wegex_match(L"\n", L"2") &&
		wegex_match(L"\n", L"3") &&
		wegex_match(L"\n", L"4") &&
		wegex_match(L"\n", L"5") &&
		wegex_match(L"\n", L"6") &&
		wegex_match(L"\n", L"7") &&
		wegex_match(L"\n", L"8") &&
		wegex_match(L"\n", L"9") &&
		wegex_match(L"+\n", L"1234") &&
		!wegex_match(L"\n", L"abc")
	);
}

bool test_wegex_hexadecimal_match() {
	return (
		wegex_match(L"\b", L"a") &&
		wegex_match(L"\b", L"b") &&
		wegex_match(L"\b", L"c") &&
		wegex_match(L"\b", L"d") &&
		wegex_match(L"\b", L"e") &&
		wegex_match(L"\b", L"f") &&
		wegex_match(L"\b", L"A") &&
		wegex_match(L"\b", L"B") &&
		wegex_match(L"\b", L"C") &&
		wegex_match(L"\b", L"D") &&
		wegex_match(L"\b", L"E") &&
		wegex_match(L"\b", L"F") &&
		wegex_match(L"\b", L"0") &&
		wegex_match(L"\b", L"1") &&
		wegex_match(L"\b", L"2") &&
		wegex_match(L"\b", L"3") &&
		wegex_match(L"\b", L"4") &&
		wegex_match(L"\b", L"5") &&
		wegex_match(L"\b", L"6") &&
		wegex_match(L"\b", L"7") &&
		wegex_match(L"\b", L"8") &&
		wegex_match(L"\b", L"9") &&
		!wegex_match(L"\b", L"z")
	);
}

bool test_wegex_ascii_alpha_match() {
	return (
		wegex_match(L"\a", L"a") &&
		wegex_match(L"\a", L"b") &&
		wegex_match(L"\a", L"c") &&
		wegex_match(L"\a", L"x") &&
		wegex_match(L"\a", L"y") &&
		wegex_match(L"\a", L"z") &&
		wegex_match(L"\a", L"A") &&
		wegex_match(L"\a", L"B") &&
		wegex_match(L"\a", L"C") &&
		wegex_match(L"\a", L"X") &&
		wegex_match(L"\a", L"Y") &&
		wegex_match(L"\a", L"Z") &&
		!wegex_match(L"\a", L"0")
	);
}

bool test_wegex_full_alpha_match() {
	return (
		wegex_match(L"\f", L"a") &&
		wegex_match(L"\f", L"b") &&
		wegex_match(L"\f", L"c") &&
		wegex_match(L"\f", L"x") &&
		wegex_match(L"\f", L"y") &&
		wegex_match(L"\f", L"z") &&
		wegex_match(L"\f", L"A") &&
		wegex_match(L"\f", L"B") &&
		wegex_match(L"\f", L"C") &&
		wegex_match(L"\f", L"X") &&
		wegex_match(L"\f", L"Y") &&
		wegex_match(L"\f", L"Z") &&
		wegex_match(L"\f", L"ß") &&
		!wegex_match(L"\f", L"0") &&
		!wegex_match(L"\f", L"9") &&
		!wegex_match(L"\f", L"!")
	);
}

bool test_wegex_optional_modifiers_at_eol() {
	return (
		wegex_match(L"x?y", L"xy") &&
		wegex_match(L"x?y", L"x") &&
		wegex_match(L"x*y", L"xyy") &&
		wegex_match(L"x*y", L"xy") &&
		wegex_match(L"x*y", L"x") &&
		wegex_match(L"x?y?z", L"xyz") &&
		wegex_match(L"x?y?z", L"xy") &&
		wegex_match(L"x?y?z", L"xz") &&
		wegex_match(L"x?y?z", L"x")
	);
}

void wegex_test_self(bool *pass) {
	tests_t tests={
		test_no_modifiers,
		test_star_modifier,
		test_plus_modifier,
		test_ternary_modifier,
		test_charset_syntax,
		test_wegex_number_match,
		test_wegex_hexadecimal_match,
		test_wegex_ascii_alpha_match,
		test_wegex_full_alpha_match,
		test_wegex_optional_modifiers_at_eol,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
