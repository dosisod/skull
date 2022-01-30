#include <stdbool.h>
#include <stdlib.h>

#include "skull/common/errors.h"
#include "skull/parse/classify.c"

#include "test/skull/parse/classify.h"
#include "test/testing.h"

static bool classify_token_fixture(const char32_t *, TokenType);
static bool classify_token_with_len_fixture(const char32_t *, TokenType, unsigned, unsigned);

static bool test_is_type_str(void) {
	Token *token = tokenize(U"Int not_a_type");

	char32_t *buf = token_to_string(token);
	char32_t *buf_next = token_to_string(token->next);

	ASSERT_TRUTHY(is_type_str(buf));
	ASSERT_FALSEY(is_type_str(buf_next));

	free_tokens(token);
	free(buf);
	free(buf_next);

	PASS
}

static bool test_is_reserved_str(void) {
	ASSERT_TRUTHY(is_reserved_str(U"return"));
	ASSERT_TRUTHY(is_reserved_str(U"mut"));
	ASSERT_FALSEY(is_reserved_str(U"not_reserved"));
	ASSERT_TRUTHY(is_reserved_str(U"unreachable"));
	ASSERT_TRUTHY(is_reserved_str(U"if"));
	ASSERT_TRUTHY(is_reserved_str(U"elif"));
	ASSERT_TRUTHY(is_reserved_str(U"else"));
	ASSERT_TRUTHY(is_reserved_str(U"while"));
	ASSERT_TRUTHY(is_reserved_str(U"external"));
	ASSERT_TRUTHY(is_reserved_str(U"export"));
	ASSERT_TRUTHY(is_reserved_str(U"noop"));
	ASSERT_TRUTHY(is_reserved_str(U"break"));
	ASSERT_TRUTHY(is_reserved_str(U"continue"));
	ASSERT_TRUTHY(is_reserved_str(U"import"));
	ASSERT_TRUTHY(is_reserved_str(U"mod"));
	ASSERT_TRUTHY(is_reserved_str(U"isnt"));
	ASSERT_TRUTHY(is_reserved_str(U"and"));
	ASSERT_TRUTHY(is_reserved_str(U"or"));
	ASSERT_TRUTHY(is_reserved_str(U"xor"));

	PASS
}

static bool test_is_constant_integer(void) {
	ASSERT_TRUTHY(is_constant_integer_str(U"123"));
	ASSERT_TRUTHY(is_constant_integer_str(U"-123"));
	ASSERT_TRUTHY(is_constant_integer_str(U"000"));
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
	ASSERT_FALSEY(is_constant_integer_str(U"000x"));
	ASSERT_FALSEY(is_constant_integer_str(U"-"));
	ASSERT_FALSEY(is_constant_integer_str(U"123aaa"));
	ASSERT_FALSEY(is_constant_integer_str(U"-123aaa"));
	ASSERT_FALSEY(is_constant_integer_str(U""));

	PASS
}

static bool test_is_constant_float(void) {
	ASSERT_TRUTHY(is_constant_float_str(U"123.0"));
	ASSERT_TRUTHY(is_constant_float_str(U"-123.0"));
	ASSERT_TRUTHY(is_constant_float_str(U"0.0"));
	ASSERT_TRUTHY(is_constant_float_str(U"Infinity"));
	ASSERT_TRUTHY(is_constant_float_str(U"-Infinity"));
	ASSERT_TRUTHY(is_constant_float_str(U"123_456.789"));

	ASSERT_FALSEY(is_constant_float_str(U"_123.0"));
	ASSERT_FALSEY(is_constant_float_str(U"123__456.0"));
	ASSERT_FALSEY(is_constant_float_str(U"123_.0"));
	ASSERT_FALSEY(is_constant_float_str(U"123"));
	ASSERT_FALSEY(is_constant_float_str(U"123.456_789"));
	ASSERT_FALSEY(is_constant_float_str(U"123."));
	ASSERT_FALSEY(is_constant_float_str(U".123"));
	ASSERT_FALSEY(is_constant_float_str(U"123aaa"));

	PASS
}

static bool test_is_constant_bool(void) {
	ASSERT_TRUTHY(is_constant_bool_str(U"true"));
	ASSERT_TRUTHY(is_constant_bool_str(U"false"));
	ASSERT_FALSEY(is_constant_bool_str(U"not_bool"));

	PASS
}

static bool test_is_constant_rune(void) {
	ASSERT_TRUTHY(is_constant_rune_str(U"'x'"));
	ASSERT_FALSEY(is_constant_rune_str(U"'x '"));
	ASSERT_FALSEY(is_constant_rune_str(U"''"));

	PASS
}

static bool test_is_constant_str(void) {
	ASSERT_TRUTHY(is_constant_str_str(U"\"\""));
	ASSERT_TRUTHY(is_constant_str_str(U"\"x\""));
	ASSERT_TRUTHY(is_constant_str_str(U"\"xyz\""));
	ASSERT_FALSEY(is_constant_str_str(U"\"bad"));
	ASSERT_FALSEY(is_constant_str_str(U"bad\""));

	PASS
}

static bool test_token_newline(void) {
	return classify_token_fixture(U"\n", TOKEN_NEWLINE);
}

static bool test_bracket_token_open(void) {
	return classify_token_fixture(U"{", TOKEN_BRACKET_OPEN);
}

static bool test_bracket_token_close(void) {
	return classify_token_fixture(U"}", TOKEN_BRACKET_CLOSE);
}

static bool test_paren_token_open(void) {
	return classify_token_fixture(U"(", TOKEN_PAREN_OPEN);
}

static bool test_paren_token_close(void) {
	return classify_token_fixture(U")", TOKEN_PAREN_CLOSE);
}

static bool test_token_mut_kw(void) {
	return classify_token_fixture(U"mut", TOKEN_KW_MUT);
}

static bool test_token_return_kw(void) {
	return classify_token_fixture(U"return", TOKEN_KW_RETURN);
}

static bool test_token_if_kw(void) {
	return classify_token_fixture(U"if", TOKEN_KW_IF);
}

static bool test_token_import_kw(void) {
	return classify_token_fixture(U"import", TOKEN_KW_IMPORT);
}

static bool test_token_equal_oper(void) {
	return classify_token_fixture(U"=", TOKEN_OPER_EQUAL);
}

static bool test_token_auto_equal_oper(void) {
	return classify_token_fixture(U":=", TOKEN_OPER_AUTO_EQUAL);
}

static bool test_token_plus_oper(void) {
	return classify_token_fixture(U"+", TOKEN_OPER_PLUS);
}

static bool test_token_minus_oper(void) {
	return classify_token_fixture(U"-", TOKEN_OPER_DASH);
}

static bool test_token_mult_oper(void) {
	return classify_token_fixture(U"*", TOKEN_OPER_STAR);
}

static bool test_token_div_oper(void) {
	return classify_token_fixture(U"/", TOKEN_OPER_DIV);
}

static bool test_token_unknown(void) {
	return classify_token_fixture(U"123garbage_value", TOKEN_UNKNOWN);
}

static bool test_token_integer_constant(void) {
	return classify_token_fixture(U"1234", TOKEN_INT_CONST);
}

static bool test_token_float_constant(void) {
	return classify_token_fixture(U"1234.0", TOKEN_FLOAT_CONST);
}

static bool test_token_bool_constant(void) {
	return classify_token_fixture(U"true", TOKEN_BOOL_CONST);
}

static bool test_token_rune_constant(void) {
	return classify_token_with_len_fixture(U"'x'", TOKEN_RUNE_CONST, 0, 3);
}

static bool test_token_rune_constant_simple_escape(void) {
	return classify_token_with_len_fixture(U"'\\r'", TOKEN_RUNE_CONST, 0, 4);
}

static bool test_token_rune_constant_hex_escape(void) {
	return classify_token_with_len_fixture(U"'\\xFF'", TOKEN_RUNE_CONST, 0, 6);
}

static bool test_token_to_string_constant(void) {
	return classify_token_with_len_fixture(U"\"xyz\"", TOKEN_STR_CONST, 0, 5);
}

static bool test_token_comment(void) {
	return classify_token_with_len_fixture(U"# this is a comment", TOKEN_COMMENT, 0, 19);
}

static bool test_token_comment_empty(void) {
	return classify_token_with_len_fixture(U"# ", TOKEN_COMMENT, 0, 2);
}

static bool test_is_valid_identifier(void) {
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

	PASS
}

static bool test_is_valid_identifier_token(void) {
	return classify_token_with_len_fixture(U"x", TOKEN_IDENTIFIER, 0, 1);
}

static bool test_new_identifier_clip_trailing_colon(void) {
	return classify_token_with_len_fixture(U"x: Int = 0", TOKEN_NEW_IDENTIFIER, 0, 1);
}

static bool test_identifier_cannot_be_type(void) {
	return classify_token_with_len_fixture(U"Int: Int = 0", TOKEN_UNKNOWN, 0, 4);
}

static bool test_identifier_cannot_be_reserved(void) {
	return classify_token_with_len_fixture(U"mut: Int = 0", TOKEN_UNKNOWN, 0, 4);
}

static bool test_classify_tokens(void) {
	Token *t = tokenize(U"{ }");
	classify_tokens(t);

	ASSERT_EQUAL(t->type, TOKEN_BRACKET_OPEN);
	ASSERT_EQUAL(t->next->type, TOKEN_BRACKET_CLOSE);

	free_tokens(t);

	PASS
}

void classifier_test_self(bool *pass) {
	RUN_ALL(
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
		test_token_import_kw,
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
		test_token_to_string_constant,
		test_token_comment,
		test_token_comment_empty,
		test_is_valid_identifier,
		test_is_valid_identifier_token,
		test_new_identifier_clip_trailing_colon,
		test_identifier_cannot_be_type,
		test_identifier_cannot_be_reserved,
		test_classify_tokens
	)

	free_errors();
}

static bool classify_token_fixture(const char32_t *code, TokenType expected) {
	Token *token = tokenize(code);
	classify_token(token);
	ASSERT_EQUAL(token->type, expected);
	free_tokens(token);

	PASS
}

static bool classify_token_with_len_fixture(const char32_t *code, TokenType tok_type, unsigned start_at, unsigned end_at) {
	Token *token = tokenize(code);
	classify_tokens(token);
	ASSERT_EQUAL(token->type, tok_type);
	ASSERT_EQUAL(token->begin, code + start_at);
	ASSERT_EQUAL(token->len, end_at);
	free_tokens(token);

	PASS
}
