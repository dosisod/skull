#include <stdbool.h>

#include "skull/common/wegex.h"
#include "skull/parse/tokenize.h"

#include "test/skull/common/wegex.h"
#include "test/testing.h"

TEST(no_modifiers, {
	return (
		wegex_match("abc", U"abc") &&
		!wegex_match("abc", U"abcd") &&
		!wegex_match("abc", U"a")
	);
})

TEST(star_modifier, {
	return (
		wegex_match("*x", U"x") &&
		wegex_match("*x", U"xxx") &&
		wegex_match("*xa", U"a") &&
		wegex_match("*xa", U"xxxxa") &&
		!wegex_match("*x", U"xa") &&
		wegex_match("*x", U"")
	);
})

TEST(plus_modifier, {
	return (
		wegex_match("+x", U"x") &&
		wegex_match("+x", U"xxx") &&
		!wegex_match("+xa", U"a") &&
		!wegex_match("+x", U"xa") &&
		!wegex_match("+x", U"")
	);
})

TEST(ternary_modifier, {
	return (
		wegex_match("?x", U"x") &&
		wegex_match("?xa", U"xa") &&
		wegex_match("?xa", U"a") &&
		wegex_match("?x", U"")
	);
})

TEST(charset_syntax, {
	return (
		wegex_match("+[abc]", U"a") &&
		wegex_match("+[abc]", U"b") &&
		wegex_match("+[abc]", U"c") &&
		wegex_match("+[abc]", U"aabbcc") &&
		!wegex_match("+[abc]", U"d") &&
		!wegex_match("+[abc]", U"ax")
	);
})

TEST(wegex_number_match, {
	return (
		wegex_match("\n", U"0") &&
		wegex_match("\n", U"1") &&
		wegex_match("\n", U"2") &&
		wegex_match("\n", U"3") &&
		wegex_match("\n", U"4") &&
		wegex_match("\n", U"5") &&
		wegex_match("\n", U"6") &&
		wegex_match("\n", U"7") &&
		wegex_match("\n", U"8") &&
		wegex_match("\n", U"9") &&
		wegex_match("+\n", U"1234") &&
		!wegex_match("\n", U"abc")
	);
})

TEST(wegex_hexadecimal_match, {
	return (
		wegex_match("\b", U"a") &&
		wegex_match("\b", U"b") &&
		wegex_match("\b", U"c") &&
		wegex_match("\b", U"d") &&
		wegex_match("\b", U"e") &&
		wegex_match("\b", U"f") &&
		wegex_match("\b", U"A") &&
		wegex_match("\b", U"B") &&
		wegex_match("\b", U"C") &&
		wegex_match("\b", U"D") &&
		wegex_match("\b", U"E") &&
		wegex_match("\b", U"F") &&
		wegex_match("\b", U"0") &&
		wegex_match("\b", U"1") &&
		wegex_match("\b", U"2") &&
		wegex_match("\b", U"3") &&
		wegex_match("\b", U"4") &&
		wegex_match("\b", U"5") &&
		wegex_match("\b", U"6") &&
		wegex_match("\b", U"7") &&
		wegex_match("\b", U"8") &&
		wegex_match("\b", U"9") &&
		!wegex_match("\b", U"z")
	);
})

TEST(wegex_ascii_alpha_match, {
	return (
		wegex_match("\a", U"a") &&
		wegex_match("\a", U"b") &&
		wegex_match("\a", U"c") &&
		wegex_match("\a", U"x") &&
		wegex_match("\a", U"y") &&
		wegex_match("\a", U"z") &&
		wegex_match("\a", U"A") &&
		wegex_match("\a", U"B") &&
		wegex_match("\a", U"C") &&
		wegex_match("\a", U"X") &&
		wegex_match("\a", U"Y") &&
		wegex_match("\a", U"Z") &&
		!wegex_match("\a", U"0")
	);
})

TEST(wegex_full_alpha_match, {
	return (
		wegex_match("\f", U"a") &&
		wegex_match("\f", U"b") &&
		wegex_match("\f", U"c") &&
		wegex_match("\f", U"x") &&
		wegex_match("\f", U"y") &&
		wegex_match("\f", U"z") &&
		wegex_match("\f", U"A") &&
		wegex_match("\f", U"B") &&
		wegex_match("\f", U"C") &&
		wegex_match("\f", U"X") &&
		wegex_match("\f", U"Y") &&
		wegex_match("\f", U"Z") &&
		wegex_match("\f", U"0") &&
		wegex_match("\f", U"9") &&
		!wegex_match("\f", U"!")
	);
})

TEST(wegex_optional_modifiers_at_eol, {
	return (
		wegex_match("x?y", U"xy") &&
		wegex_match("x?y", U"x") &&
		wegex_match("x*y", U"xyy") &&
		wegex_match("x*y", U"xy") &&
		wegex_match("x*y", U"x") &&
		wegex_match("x?y?z", U"xyz") &&
		wegex_match("x?y?z", U"xy") &&
		wegex_match("x?y?z", U"xz") &&
		wegex_match("x?y?z", U"x")
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
