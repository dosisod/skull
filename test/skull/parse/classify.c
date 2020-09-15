#include <stdbool.h>

#include "skull/parse/classify.h"

#include "test/skull/parse/classify.h"
#include "test/testing.h"

TEST(is_type_str, {
	const char32_t *code=U"int not_a_type";
	Token *token=tokenize(code);

	char32_t *buf=token_str(token);
	char32_t *buf_next=token_str(token->next);

	const bool pass=(
		is_type_str(buf) &&
		!is_type_str(buf_next)
	);

	free_tokens(token);
	free(buf);
	free(buf_next);
	return pass;
})

TEST(is_keyword_str, {
	return (
		is_keyword_str(U"return") &&
		is_keyword_str(U"mut") &&
		!is_keyword_str(U"not_a_keyword")
	);
})

TEST(is_constant_integer, {
	return (
		is_constant_integer_str(U"123") &&
		is_constant_integer_str(U"-123") &&
		is_constant_integer_str(U"0x123") &&
		is_constant_integer_str(U"0x0123456789ABCDEF") &&
		is_constant_integer_str(U"0xabcdef") &&
		is_constant_integer_str(U"0b10101") &&
		is_constant_integer_str(U"0o01234567") &&
		!is_constant_integer_str(U"0b") &&
		!is_constant_integer_str(U"0x") &&
		!is_constant_integer_str(U"0o") &&
		!is_constant_integer_str(U"-") &&
		!is_constant_integer_str(U"123aaa") &&
		!is_constant_integer_str(U"-123aaa") &&
		!is_constant_integer_str(U"")
	);
})

TEST(is_constant_float, {
	return (
		is_constant_float_str(U"123.0") &&
		is_constant_float_str(U"-123.0") &&
		is_constant_float_str(U"0.0") &&
		is_constant_float_str(U"Infinity") &&
		is_constant_float_str(U"-Infinity") &&
		!is_constant_float_str(U"123") &&
		!is_constant_float_str(U"123.") &&
		!is_constant_float_str(U".123") &&
		!is_constant_float_str(U"123aaa")
	);
})

TEST(is_constant_bool, {
	return (
		is_constant_bool_str(U"true") &&
		is_constant_bool_str(U"false") &&
		!is_constant_bool_str(U"not_bool")
	);
})

TEST(is_constant_rune, {
	return (
		is_constant_rune_str(U"'x'") &&
		!is_constant_rune_str(U"'x '") &&
		!is_constant_rune_str(U"''")
	);
})

TEST(is_constant_str, {
	return (
		is_constant_str_str(U"\"\"") &&
		is_constant_str_str(U"\"x\"") &&
		is_constant_str_str(U"\"xyz\"") &&
		!is_constant_str_str(U"\"bad") &&
		!is_constant_str_str(U"bad\"")
	);
})

TEST(token_newline, {
	TEST_CLASSIFY_TOKEN(U"\n", TOKEN_NEWLINE);
})

TEST(bracket_token_open, {
	TEST_CLASSIFY_TOKEN(U"[", TOKEN_BRACKET_OPEN);
})

TEST(bracket_token_close, {
	TEST_CLASSIFY_TOKEN(U"]", TOKEN_BRACKET_CLOSE);
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

TEST(token_import_kw, {
	TEST_CLASSIFY_TOKEN(U"import", TOKEN_KW_IMPORT);
})

TEST(token_from_kw, {
	TEST_CLASSIFY_TOKEN(U"from", TOKEN_KW_FROM);
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

TEST(token_type, {
	const char32_t *code=U"int not_a_type";
	Token *t=tokenize(code);
	classify_tokens(t);

	const bool pass=(
		t->token_type==TOKEN_TYPE &&
		t->next->token_type!=TOKEN_TYPE
	);

	free_tokens(t);
	return pass;
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
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"'x'", TOKEN_RUNE_CONST, 1, 2);
})

TEST(token_str_constant, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"\"xyz\"", TOKEN_STR_CONST, 1, 4);
})

TEST(token_Typeype, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"int", TOKEN_TYPE, 0, 3);
})

TEST(token_comment, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"# this is a comment", TOKEN_COMMENT, 0, 19);
})

TEST(token_comment_empty, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"#", TOKEN_COMMENT, 0, 1);
})

TEST(is_valid_identifier, {
	return (
		is_valid_identifier_str(U"a") &&
		is_valid_identifier_str(U"z") &&
		is_valid_identifier_str(U"A") &&
		is_valid_identifier_str(U"Z") &&
		!is_valid_identifier_str(U"0") &&
		!is_valid_identifier_str(U"_") &&
		!is_valid_identifier_str(U"~") &&
		is_valid_identifier_str(U"a:") &&
		is_valid_identifier_str(U"abc:") &&
		!is_valid_identifier_str(U"1var") &&
		is_valid_identifier_str(U"var1") &&
		is_valid_identifier_str(U"x1") &&
		is_valid_identifier_str(U"x_") &&
		!is_valid_identifier_str(U"_x")
	);
})

TEST(is_valid_identifier_token, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"x", TOKEN_IDENTIFIER, 0, 1);
})

TEST(new_identifier_clip_trailing_colon, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"x: int = 0", TOKEN_NEW_IDENTIFIER, 0, 1);
})

TEST(identifier_cannot_be_type, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"int: int = 0", TOKEN_UNKNOWN, 0, 4);
})

TEST(identifier_cannot_be_keyword, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"mut: int = 0", TOKEN_UNKNOWN, 0, 4);
})

TEST(identifier_cannot_be_func_name, {
	TEST_CLASSIFY_TOKEN_WITH_LEN(U"print: int = 0", TOKEN_UNKNOWN, 0, 6);
})

TEST(classify_tokens, {
	const char32_t *code=U"[ ]";
	Token *t=tokenize(code);
	classify_tokens(t);

	const bool pass=(
		t->token_type==TOKEN_BRACKET_OPEN &&
		t->next->token_type==TOKEN_BRACKET_CLOSE
	);

	free(t->next);
	free(t);

	return pass;
})

void classifier_test_self(bool *pass) {
	tests_t tests={
		test_is_type_str,
		test_is_keyword_str,
		test_is_constant_integer,
		test_is_constant_float,
		test_is_constant_bool,
		test_is_constant_rune,
		test_is_constant_str,
		test_token_newline,
		test_bracket_token_open,
		test_bracket_token_close,
		test_token_mut_kw,
		test_token_return_kw,
		test_token_if_kw,
		test_token_import_kw,
		test_token_from_kw,
		test_token_equal_oper,
		test_token_plus_oper,
		test_token_minus_oper,
		test_token_mult_oper,
		test_token_div_oper,
		test_token_auto_equal_oper,
		test_token_type,
		test_token_unknown,
		test_token_integer_constant,
		test_token_float_constant,
		test_token_bool_constant,
		test_token_rune_constant,
		test_token_str_constant,
		test_token_Typeype,
		test_token_comment,
		test_token_comment_empty,
		test_is_valid_identifier,
		test_is_valid_identifier_token,
		test_new_identifier_clip_trailing_colon,
		test_identifier_cannot_be_type,
		test_identifier_cannot_be_keyword,
		test_identifier_cannot_be_func_name,
		test_classify_tokens,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
