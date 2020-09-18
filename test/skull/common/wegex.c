#include "skull/common/wegex.h"
#include "skull/parse/tokenize.h"

#include "test/skull/common/wegex.h"
#include "test/testing.h"

TEST(no_modifiers, {
	ASSERT_TRUTHY(wegex_match("abc", U"abc"));
	ASSERT_FALSEY(wegex_match("abc", U"abcd"));
	ASSERT_FALSEY(wegex_match("abc", U"a"));
})

TEST(star_modifier, {
	ASSERT_TRUTHY(wegex_match("*x", U"x"));
	ASSERT_TRUTHY(wegex_match("*x", U"xxx"));
	ASSERT_TRUTHY(wegex_match("*xa", U"a"));
	ASSERT_TRUTHY(wegex_match("*xa", U"xxxxa"));
	ASSERT_TRUTHY(wegex_match("*x", U""));
	ASSERT_FALSEY(wegex_match("*x", U"xa"));
})

TEST(plus_modifier, {
	ASSERT_TRUTHY(wegex_match("+x", U"x"));
	ASSERT_TRUTHY(wegex_match("+x", U"xxx"));
	ASSERT_FALSEY(wegex_match("+xa", U"a"));
	ASSERT_FALSEY(wegex_match("+x", U"xa"));
	ASSERT_FALSEY(wegex_match("+x", U""));
})

TEST(ternary_modifier, {
	ASSERT_TRUTHY(wegex_match("?x", U"x"));
	ASSERT_TRUTHY(wegex_match("?xa", U"xa"));
	ASSERT_TRUTHY(wegex_match("?xa", U"a"));
	ASSERT_TRUTHY(wegex_match("?x", U""));
})

TEST(charset_syntax, {
	ASSERT_TRUTHY(wegex_match("+[abc]", U"a"));
	ASSERT_TRUTHY(wegex_match("+[abc]", U"b"));
	ASSERT_TRUTHY(wegex_match("+[abc]", U"c"));
	ASSERT_TRUTHY(wegex_match("+[abc]", U"aabbcc"));
	ASSERT_FALSEY(wegex_match("+[abc]", U"d"));
	ASSERT_FALSEY(wegex_match("+[abc]", U"ax"));
})

TEST(wegex_number_match, {
	ASSERT_TRUTHY(wegex_match("\n", U"0"));
	ASSERT_TRUTHY(wegex_match("\n", U"1"));
	ASSERT_TRUTHY(wegex_match("\n", U"2"));
	ASSERT_TRUTHY(wegex_match("\n", U"3"));
	ASSERT_TRUTHY(wegex_match("\n", U"4"));
	ASSERT_TRUTHY(wegex_match("\n", U"5"));
	ASSERT_TRUTHY(wegex_match("\n", U"6"));
	ASSERT_TRUTHY(wegex_match("\n", U"7"));
	ASSERT_TRUTHY(wegex_match("\n", U"8"));
	ASSERT_TRUTHY(wegex_match("\n", U"9"));
	ASSERT_TRUTHY(wegex_match("+\n", U"1234"));
	ASSERT_FALSEY(wegex_match("\n", U"abc"));
})

TEST(wegex_hexadecimal_match, {
	ASSERT_TRUTHY(wegex_match("\b", U"a"));
	ASSERT_TRUTHY(wegex_match("\b", U"b"));
	ASSERT_TRUTHY(wegex_match("\b", U"c"));
	ASSERT_TRUTHY(wegex_match("\b", U"d"));
	ASSERT_TRUTHY(wegex_match("\b", U"e"));
	ASSERT_TRUTHY(wegex_match("\b", U"f"));
	ASSERT_TRUTHY(wegex_match("\b", U"A"));
	ASSERT_TRUTHY(wegex_match("\b", U"B"));
	ASSERT_TRUTHY(wegex_match("\b", U"C"));
	ASSERT_TRUTHY(wegex_match("\b", U"D"));
	ASSERT_TRUTHY(wegex_match("\b", U"E"));
	ASSERT_TRUTHY(wegex_match("\b", U"F"));
	ASSERT_TRUTHY(wegex_match("\b", U"0"));
	ASSERT_TRUTHY(wegex_match("\b", U"1"));
	ASSERT_TRUTHY(wegex_match("\b", U"2"));
	ASSERT_TRUTHY(wegex_match("\b", U"3"));
	ASSERT_TRUTHY(wegex_match("\b", U"4"));
	ASSERT_TRUTHY(wegex_match("\b", U"5"));
	ASSERT_TRUTHY(wegex_match("\b", U"6"));
	ASSERT_TRUTHY(wegex_match("\b", U"7"));
	ASSERT_TRUTHY(wegex_match("\b", U"8"));
	ASSERT_TRUTHY(wegex_match("\b", U"9"));
	ASSERT_FALSEY(wegex_match("\b", U"z"));
})

TEST(wegex_ascii_alpha_match, {
	ASSERT_TRUTHY(wegex_match("\a", U"a"));
	ASSERT_TRUTHY(wegex_match("\a", U"b"));
	ASSERT_TRUTHY(wegex_match("\a", U"c"));
	ASSERT_TRUTHY(wegex_match("\a", U"x"));
	ASSERT_TRUTHY(wegex_match("\a", U"y"));
	ASSERT_TRUTHY(wegex_match("\a", U"z"));
	ASSERT_TRUTHY(wegex_match("\a", U"A"));
	ASSERT_TRUTHY(wegex_match("\a", U"B"));
	ASSERT_TRUTHY(wegex_match("\a", U"C"));
	ASSERT_TRUTHY(wegex_match("\a", U"X"));
	ASSERT_TRUTHY(wegex_match("\a", U"Y"));
	ASSERT_TRUTHY(wegex_match("\a", U"Z"));
	ASSERT_FALSEY(wegex_match("\a", U"0"));
})

TEST(wegex_full_alpha_match, {
	ASSERT_TRUTHY(wegex_match("\f", U"a"));
	ASSERT_TRUTHY(wegex_match("\f", U"b"));
	ASSERT_TRUTHY(wegex_match("\f", U"c"));
	ASSERT_TRUTHY(wegex_match("\f", U"x"));
	ASSERT_TRUTHY(wegex_match("\f", U"y"));
	ASSERT_TRUTHY(wegex_match("\f", U"z"));
	ASSERT_TRUTHY(wegex_match("\f", U"A"));
	ASSERT_TRUTHY(wegex_match("\f", U"B"));
	ASSERT_TRUTHY(wegex_match("\f", U"C"));
	ASSERT_TRUTHY(wegex_match("\f", U"X"));
	ASSERT_TRUTHY(wegex_match("\f", U"Y"));
	ASSERT_TRUTHY(wegex_match("\f", U"Z"));
	ASSERT_TRUTHY(wegex_match("\f", U"0"));
	ASSERT_TRUTHY(wegex_match("\f", U"9"));
	ASSERT_FALSEY(wegex_match("\f", U"!"));
})

TEST(wegex_optional_modifiers_at_eol, {
	ASSERT_TRUTHY(wegex_match("x?y", U"xy"));
	ASSERT_TRUTHY(wegex_match("x?y", U"x"));
	ASSERT_TRUTHY(wegex_match("x*y", U"xyy"));
	ASSERT_TRUTHY(wegex_match("x*y", U"xy"));
	ASSERT_TRUTHY(wegex_match("x*y", U"x"));
	ASSERT_TRUTHY(wegex_match("x?y?z", U"xyz"));
	ASSERT_TRUTHY(wegex_match("x?y?z", U"xy"));
	ASSERT_TRUTHY(wegex_match("x?y?z", U"xz"));
	ASSERT_TRUTHY(wegex_match("x?y?z", U"x"));
})

TEST_SELF(wegex,
	test_no_modifiers,
	test_star_modifier,
	test_plus_modifier,
	test_ternary_modifier,
	test_charset_syntax,
	test_wegex_number_match,
	test_wegex_hexadecimal_match,
	test_wegex_ascii_alpha_match,
	test_wegex_full_alpha_match,
	test_wegex_optional_modifiers_at_eol
)
