#include <stdbool.h>

#include "../../src/parse/tokenize.h"
#include "../../src/common/wegex.h"
#include "../../test/testing.h"

TEST(no_modifiers, {
	return (
		wegex_match(U"abc", U"abc") &&
		!wegex_match(U"abc", U"abcd") &&
		!wegex_match(U"abc", U"a")
	);
})

TEST(star_modifier, {
	return (
		wegex_match(U"*x", U"x") &&
		wegex_match(U"*x", U"xxx") &&
		wegex_match(U"*xa", U"a") &&
		wegex_match(U"*xa", U"xxxxa") &&
		!wegex_match(U"*x", U"xa") &&
		wegex_match(U"*x", U"")
	);
})

TEST(plus_modifier, {
	return (
		wegex_match(U"+x", U"x") &&
		wegex_match(U"+x", U"xxx") &&
		!wegex_match(U"+xa", U"a") &&
		!wegex_match(U"+x", U"xa") &&
		!wegex_match(U"+x", U"")
	);
})

TEST(ternary_modifier, {
	return (
		wegex_match(U"?x", U"x") &&
		wegex_match(U"?xa", U"xa") &&
		wegex_match(U"?xa", U"a") &&
		wegex_match(U"?x", U"")
	);
})

TEST(charset_syntax, {
	return (
		wegex_match(U"+[abc]", U"a") &&
		wegex_match(U"+[abc]", U"b") &&
		wegex_match(U"+[abc]", U"c") &&
		wegex_match(U"+[abc]", U"aabbcc") &&
		!wegex_match(U"+[abc]", U"d") &&
		!wegex_match(U"+[abc]", U"ax")
	);
})

TEST(wegex_number_match, {
	return (
		wegex_match(U"\n", U"0") &&
		wegex_match(U"\n", U"1") &&
		wegex_match(U"\n", U"2") &&
		wegex_match(U"\n", U"3") &&
		wegex_match(U"\n", U"4") &&
		wegex_match(U"\n", U"5") &&
		wegex_match(U"\n", U"6") &&
		wegex_match(U"\n", U"7") &&
		wegex_match(U"\n", U"8") &&
		wegex_match(U"\n", U"9") &&
		wegex_match(U"+\n", U"1234") &&
		!wegex_match(U"\n", U"abc")
	);
})

TEST(wegex_hexadecimal_match, {
	return (
		wegex_match(U"\b", U"a") &&
		wegex_match(U"\b", U"b") &&
		wegex_match(U"\b", U"c") &&
		wegex_match(U"\b", U"d") &&
		wegex_match(U"\b", U"e") &&
		wegex_match(U"\b", U"f") &&
		wegex_match(U"\b", U"A") &&
		wegex_match(U"\b", U"B") &&
		wegex_match(U"\b", U"C") &&
		wegex_match(U"\b", U"D") &&
		wegex_match(U"\b", U"E") &&
		wegex_match(U"\b", U"F") &&
		wegex_match(U"\b", U"0") &&
		wegex_match(U"\b", U"1") &&
		wegex_match(U"\b", U"2") &&
		wegex_match(U"\b", U"3") &&
		wegex_match(U"\b", U"4") &&
		wegex_match(U"\b", U"5") &&
		wegex_match(U"\b", U"6") &&
		wegex_match(U"\b", U"7") &&
		wegex_match(U"\b", U"8") &&
		wegex_match(U"\b", U"9") &&
		!wegex_match(U"\b", U"z")
	);
})

TEST(wegex_ascii_alpha_match, {
	return (
		wegex_match(U"\a", U"a") &&
		wegex_match(U"\a", U"b") &&
		wegex_match(U"\a", U"c") &&
		wegex_match(U"\a", U"x") &&
		wegex_match(U"\a", U"y") &&
		wegex_match(U"\a", U"z") &&
		wegex_match(U"\a", U"A") &&
		wegex_match(U"\a", U"B") &&
		wegex_match(U"\a", U"C") &&
		wegex_match(U"\a", U"X") &&
		wegex_match(U"\a", U"Y") &&
		wegex_match(U"\a", U"Z") &&
		!wegex_match(U"\a", U"0")
	);
})

TEST(wegex_full_alpha_match, {
	return (
		wegex_match(U"\f", U"a") &&
		wegex_match(U"\f", U"b") &&
		wegex_match(U"\f", U"c") &&
		wegex_match(U"\f", U"x") &&
		wegex_match(U"\f", U"y") &&
		wegex_match(U"\f", U"z") &&
		wegex_match(U"\f", U"A") &&
		wegex_match(U"\f", U"B") &&
		wegex_match(U"\f", U"C") &&
		wegex_match(U"\f", U"X") &&
		wegex_match(U"\f", U"Y") &&
		wegex_match(U"\f", U"Z") &&
		wegex_match(U"\f", U"0") &&
		wegex_match(U"\f", U"9") &&
		!wegex_match(U"\f", U"!")
	);
})

TEST(wegex_optional_modifiers_at_eol, {
	return (
		wegex_match(U"x?y", U"xy") &&
		wegex_match(U"x?y", U"x") &&
		wegex_match(U"x*y", U"xyy") &&
		wegex_match(U"x*y", U"xy") &&
		wegex_match(U"x*y", U"x") &&
		wegex_match(U"x?y?z", U"xyz") &&
		wegex_match(U"x?y?z", U"xy") &&
		wegex_match(U"x?y?z", U"xz") &&
		wegex_match(U"x?y?z", U"x")
	);
})

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
