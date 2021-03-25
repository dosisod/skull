#include <stdbool.h>
#include <stdlib.h>

#include "skull/parse/classify.h"

#include "test/skull/parse/classify.h"
#include "test/testing.h"

TEST(is_type_str, {
	Token *token = tokenize(U"Int not_a_type");

	char32_t *buf = token_str(token);
	char32_t *buf_next = token_str(token->next);

	ASSERT_TRUTHY(is_type_str(buf));
	ASSERT_FALSEY(is_type_str(buf_next));

	free_tokens(token);
	free(buf);
	free(buf_next);
})

TEST(is_reserved_str, {
	ASSERT_TRUTHY(is_reserved_str(U"return"));
	ASSERT_TRUTHY(is_reserved_str(U"mut"));
	ASSERT_FALSEY(is_reserved_str(U"not_reserved"));
})

TEST(is_constant_integer, {
	ASSERT_TRUTHY(is_constant_integer_str(U"123"));
	ASSERT_TRUTHY(is_constant_integer_str(U"-123"));
	ASSERT_TRUTHY(is_constant_integer_str(U"0x123"));
	ASSERT_TRUTHY(is_constant_integer_str(U"0x0123456789ABCDEF"));
	ASSERT_TRUTHY(is_constant_integer_str(U"0xabcdef"));
	ASSERT_TRUTHY(is_constant_integer_str(U"0b10101"));
	ASSERT_TRUTHY(is_constant_integer_str(U"0o01234567"));
	ASSERT_TRUTHY(is_constant_integer_str(U"123_456"));
	ASSERT_TRUTHY(is_constant_integer_str(U"123_456_789"));

	ASSERT_FALSEY(is_constant_integer_str(U"_123"));
	ASSERT_FALSEY(is_constant_integer_str(U"123__456"));
	ASSERT_FALSEY(is_constant_integer_str(U"123_"));
	ASSERT_FALSEY(is_constant_integer_str(U"0b"));
	ASSERT_FALSEY(is_constant_integer_str(U"0x"));
	ASSERT_FALSEY(is_constant_integer_str(U"0o"));
	ASSERT_FALSEY(is_constant_integer_str(U"-"));
	ASSERT_FALSEY(is_constant_integer_str(U"123aaa"));
	ASSERT_FALSEY(is_constant_integer_str(U"-123aaa"));
	ASSERT_FALSEY(is_constant_integer_str(U""));
})

TEST(is_constant_float, {
	ASSERT_TRUTHY(is_constant_float_str(U"123.0"));
	ASSERT_TRUTHY(is_constant_float_str(U"-123.0"));
	ASSERT_TRUTHY(is_constant_float_str(U"0.0"));
	ASSERT_TRUTHY(is_constant_float_str(U"Infinity"));
	ASSERT_TRUTHY(is_constant_float_str(U"-Infinity"));
	ASSERT_FALSEY(is_constant_float_str(U"123"));
	ASSERT_FALSEY(is_constant_float_str(U"123."));
	ASSERT_FALSEY(is_constant_float_str(U".123"));
	ASSERT_FALSEY(is_constant_float_str(U"123aaa"));
})

TEST(is_constant_bool, {
	ASSERT_TRUTHY(is_constant_bool_str(U"true"));
	ASSERT_TRUTHY(is_constant_bool_str(U"false"));
	ASSERT_FALSEY(is_constant_bool_str(U"not_bool"));
})

TEST(is_constant_rune, {
	ASSERT_TRUTHY(is_constant_rune_str(U"'x'"));
	ASSERT_FALSEY(is_constant_rune_str(U"'x '"));
	ASSERT_FALSEY(is_constant_rune_str(U"''"));
})

TEST(is_constant_str, {
	ASSERT_TRUTHY(is_constant_str_str(U"\"\""));
	ASSERT_TRUTHY(is_constant_str_str(U"\"x\""));
	ASSERT_TRUTHY(is_constant_str_str(U"\"xyz\""));
	ASSERT_FALSEY(is_constant_str_str(U"\"bad"));
	ASSERT_FALSEY(is_constant_str_str(U"bad\""));
})

TEST(token_newline, {
	TEST_CLASSIFY_TOKEN(U"\n", TOKEN_NEWLINE);
})

TEST(bracket_token_open, {
	TEST_CLASSIFY_TOKEN(U"{", TOKEN_BRACKET_OPEN);
})

TEST(bracket_token_close, {
	TEST_CLASSIFY_TOKEN(U"}", TOKEN_BRACKET_CLOSE);
})

TEST(paren_token_open, {
	TEST_CLASSIFY_TOKEN(U"(", TOKEN_PAREN_OPEN);
})

TEST(paren_token_close, {
	TEST_CLASSIFY_TOKEN(U")", TOKEN_PAREN_CLOSE);
})

TEST(token_mut_kw, {
	TEST_CLASSIFY_TOKEN(U"mut", TOKEN_KW_MUT);
})

TEST(token_return_kw, {
	TEST_CLASSIFY_TOKEN(U"return", TOKEN_KW_RETURN);
})

TEST(token_if_kw, {
	TEST_CLASSIFY_TOKEN(U"if", TOKEN_KW_IF);
})

TEST(token_equal_oper, {
	TEST_CLASSIFY_TOKEN(U"=", TOKEN_OPER_EQUAL);
})

TEST(token_auto_equal_oper, {
	TEST_CLASSIFY_TOKEN(U":=", TOKEN_OPER_AUTO_EQUAL);
})

TEST(token_plus_oper, {
	TEST_CLASSIFY_TOKEN(U"+", TOKEN_OPER_PLUS);
})

TEST(token_minus_oper, {
	TEST_CLASSIFY_TOKEN(U"-", TOKEN_OPER_MINUS);
})

TEST(token_mult_oper, {
	TEST_CLASSIFY_TOKEN(U"*", TOKEN_OPER_MULT);
})

TEST(token_div_oper, {
	TEST_CLASSIFY_TOKEN(U"/", TOKEN_OPER_DIV);
})

TEST(token_unknown, {
	TEST_CLASSIFY_TOKEN(U"123garbage_value", TOKEN_UNKNOWN);
})

TEST(token_integer_constant, {
	TEST_CLASSIFY_TOKEN(U"1234", TOKEN_INT_CONST);
})

TEST(token_float_constant, {
	TEST_CLASSIFY_TOKEN(U"1234.0", TOKEN_FLOAT_CONST);
})

TEST(token_bool_constant, {
	TEST_CLASSIFY_TOKEN(U"true", TOKEN_BOOL_CONST);
})

TEST(token_rune_constant, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"'x'", TOKEN_RUNE_CONST, 0, 3);
})

TEST(token_rune_constant_simple_escape, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"'\\r'", TOKEN_RUNE_CONST, 0, 4);
})

TEST(token_rune_constant_hex_escape, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"'\\xFF'", TOKEN_RUNE_CONST, 0, 6);
})

TEST(token_str_constant, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"\"xyz\"", TOKEN_STR_CONST, 0, 5);
})

TEST(token_type, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"Int", TOKEN_TYPE, 0, 3);
})

TEST(token_comment, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"# this is a comment", TOKEN_COMMENT, 0, 19);
})

TEST(token_comment_empty, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"# ", TOKEN_COMMENT, 0, 2);
})

TEST(is_valid_identifier, {
	ASSERT_TRUTHY(is_valid_identifier_str(U"a"));
	ASSERT_TRUTHY(is_valid_identifier_str(U"z"));
	ASSERT_TRUTHY(is_valid_identifier_str(U"A"));
	ASSERT_TRUTHY(is_valid_identifier_str(U"Z"));
	ASSERT_FALSEY(is_valid_identifier_str(U"0"));
	ASSERT_FALSEY(is_valid_identifier_str(U"_"));
	ASSERT_FALSEY(is_valid_identifier_str(U"~"));
	ASSERT_TRUTHY(is_valid_identifier_str(U"a:"));
	ASSERT_TRUTHY(is_valid_identifier_str(U"abc:"));
	ASSERT_FALSEY(is_valid_identifier_str(U"1var"));
	ASSERT_TRUTHY(is_valid_identifier_str(U"var1"));
	ASSERT_TRUTHY(is_valid_identifier_str(U"x1"));
	ASSERT_TRUTHY(is_valid_identifier_str(U"x_"));
	ASSERT_FALSEY(is_valid_identifier_str(U"_x"));
})

TEST(is_valid_identifier_token, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"x", TOKEN_IDENTIFIER, 0, 1);
})

TEST(new_identifier_clip_trailing_colon, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"x: Int = 0", TOKEN_NEW_IDENTIFIER, 0, 1);
})

TEST(identifier_cannot_be_type, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"Int: Int = 0", TOKEN_UNKNOWN, 0, 4);
})

TEST(identifier_cannot_be_reserved, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"mut: Int = 0", TOKEN_UNKNOWN, 0, 4);
})

TEST(classify_tokens, {
	Token *t = tokenize(U"{ }");
	classify_tokens(t);

	ASSERT_EQUAL(t->type, TOKEN_BRACKET_OPEN);
	ASSERT_EQUAL(t->next->type, TOKEN_BRACKET_CLOSE);

	free_tokens(t);
})

TEST_SELF(classifier,
	test_is_type_str,
	test_is_reserved_str,
	test_is_constant_integer,
	test_is_constant_float,
	test_is_constant_bool,
	test_is_constant_rune,
	test_is_constant_str,
	test_token_newline,
	test_bracket_token_open,
	test_bracket_token_close,
	test_paren_token_open,
	test_paren_token_close,
	test_token_mut_kw,
	test_token_return_kw,
	test_token_if_kw,
	test_token_equal_oper,
	test_token_plus_oper,
	test_token_minus_oper,
	test_token_mult_oper,
	test_token_div_oper,
	test_token_auto_equal_oper,
	test_token_unknown,
	test_token_integer_constant,
	test_token_float_constant,
	test_token_bool_constant,
	test_token_rune_constant,
	test_token_rune_constant_simple_escape,
	test_token_rune_constant_hex_escape,
	test_token_str_constant,
	test_token_type,
	test_token_comment,
	test_token_comment_empty,
	test_is_valid_identifier,
	test_is_valid_identifier_token,
	test_new_identifier_clip_trailing_colon,
	test_identifier_cannot_be_type,
	test_identifier_cannot_be_reserved,
	test_classify_tokens
)
