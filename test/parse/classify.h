#include <stdbool.h>

#include "../../src/parse/classify.h"
#include "../../test/testing.h"

TEST(is_type_token, {
	const wchar_t *code=L"int not_a_type";
	token_t *token=tokenize(code);

	make_default_types();

	const bool pass=(
		is_type_token(token) &&
		!is_type_token(token->next)
	);

	free_types();
	free_tokens(token);
	return pass;
});

TEST(is_keyword_str, {
	return (
		is_keyword_str(L"return") &&
		is_keyword_str(L"mut") &&
		!is_keyword_str(L"not_a_keyword")
	);
});

TEST(is_function_token, {
	const wchar_t *code=L"main[] ->";
	token_t *token=tokenize(code);

	const bool pass=(
		!is_function_token(token) &&
		is_function_token(token->next->next->next)
	);

	free_tokens(token);
	return pass;
});

TEST(is_constant_integer, {
	return (
		is_constant_integer(L"123") &&
		is_constant_integer(L"-123") &&
		is_constant_integer(L"0x123") &&
		is_constant_integer(L"0x0123456789ABCDEF") &&
		is_constant_integer(L"0xabcdef") &&
		is_constant_integer(L"0b10101") &&
		is_constant_integer(L"0o01234567") &&
		!is_constant_integer(L"0b") &&
		!is_constant_integer(L"0x") &&
		!is_constant_integer(L"0o") &&
		!is_constant_integer(L"-") &&
		!is_constant_integer(L"123aaa") &&
		!is_constant_integer(L"-123aaa") &&
		!is_constant_integer(L"")
	);
});

TEST(is_constant_float, {
	return (
		is_constant_float(L"123.0") &&
		is_constant_float(L"-123.0") &&
		is_constant_float(L"0.0") &&
		!is_constant_float(L"123") &&
		!is_constant_float(L"123.") &&
		!is_constant_float(L".123") &&
		!is_constant_float(L"123aaa")
	);
});

TEST(is_constant_bool, {
	return (
		is_constant_bool(L"true") &&
		is_constant_bool(L"false") &&
		!is_constant_bool(L"not_bool")
	);
});

TEST(is_constant_char, {
	return (
		is_constant_char(L"'x'") &&
		!is_constant_char(L"'x '") &&
		!is_constant_char(L"''")
	);
});

TEST(is_constant_str, {
	return (
		is_constant_str(L"\"\"") &&
		is_constant_str(L"\"x\"") &&
		is_constant_str(L"\"xyz\"") &&
		!is_constant_str(L"\"bad") &&
		!is_constant_str(L"bad\"")
	);
});

#define TEST_CLASSIFY_TOKEN(str, expected) \
	const wchar_t *code=str; \
	token_t *token=tokenize(code); \
	classify_token(token); \
	const bool pass=(token->token_type==(expected)); \
	free(token); \
	return pass;

TEST(bracket_token_open, {
	TEST_CLASSIFY_TOKEN(L"[", TOKEN_BRACKET_OPEN);
});

TEST(bracket_token_close, {
	TEST_CLASSIFY_TOKEN(L"]", TOKEN_BRACKET_CLOSE);
});

TEST(token_mut_kw, {
	TEST_CLASSIFY_TOKEN(L"mut", TOKEN_KW_MUT);
});

TEST(token_return_kw, {
	TEST_CLASSIFY_TOKEN(L"return", TOKEN_KW_RETURN);
});

TEST(token_or_kw, {
	TEST_CLASSIFY_TOKEN(L"or", TOKEN_KW_OR);
});

TEST(token_and_kw, {
	TEST_CLASSIFY_TOKEN(L"and", TOKEN_KW_AND);
});

TEST(token_not_kw, {
	TEST_CLASSIFY_TOKEN(L"not", TOKEN_KW_NOT);
});

TEST(token_equal_oper, {
	TEST_CLASSIFY_TOKEN(L"=", TOKEN_OPER_EQUAL);
});

TEST(token_equal_equal_oper, {
	TEST_CLASSIFY_TOKEN(L"==", TOKEN_OPER_EQUAL_EQUAL);
});

TEST(token_not_equal_oper, {
	TEST_CLASSIFY_TOKEN(L"!=", TOKEN_OPER_NOT_EQUAL);
});

TEST(token_auto_equal_oper, {
	TEST_CLASSIFY_TOKEN(L":=", TOKEN_OPER_AUTO_EQUAL);
});

TEST(token_plus_oper, {
	TEST_CLASSIFY_TOKEN(L"+", TOKEN_OPER_PLUS);
});

TEST(token_type, {
	const wchar_t *code=L"int not_a_type";
	token_t *t=tokenize(code);
	make_default_types();
	classify_tokens(t);

	const bool pass=(
		t->token_type==TOKEN_TYPE &&
		t->next->token_type!=TOKEN_TYPE
	);

	free_types();
	free_tokens(t);
	return pass;
});

TEST(token_function, {
	TEST_CLASSIFY_TOKEN(L"->", TOKEN_FUNCTION);
});

TEST(token_unknown, {
	TEST_CLASSIFY_TOKEN(L"123garbage_value", TOKEN_UNKNOWN);
});

TEST(token_integer_constant, {
	TEST_CLASSIFY_TOKEN(L"1234", TOKEN_INT_CONST);
});

TEST(token_float_constant, {
	TEST_CLASSIFY_TOKEN(L"1234.0", TOKEN_FLOAT_CONST);
});

TEST(token_bool_constant, {
	TEST_CLASSIFY_TOKEN(L"true", TOKEN_BOOL_CONST);
});

#undef TEST_CLASSIFY_TOKEN

TEST(token_char_constant, {
	const wchar_t *code=L"'x'";
	token_t *t=tokenize(code);
	classify_tokens(t);

	const bool pass=(
		t->token_type==TOKEN_CHAR_CONST &&
		t->begin==(code + 1) &&
		t->end==(code + 2)
	);

	free(t);
	return pass;
});

TEST(token_str_constant, {
	const wchar_t *code=L"\"xyz\"";
	token_t *t=tokenize(code);
	classify_tokens(t);

	const bool pass=(
		t->token_type==TOKEN_STR_CONST &&
		t->begin==(code + 1) &&
		t->end==(code + 4)
	);

	free(t);
	return pass;
});

TEST(is_valid_identifier, {
	return (
		is_valid_identifier(L"a") &&
		is_valid_identifier(L"z") &&
		is_valid_identifier(L"A") &&
		is_valid_identifier(L"Z") &&
		!is_valid_identifier(L"0") &&
		!is_valid_identifier(L"_") &&
		!is_valid_identifier(L"~") &&
		is_valid_identifier(L"a:") &&
		is_valid_identifier(L"abc:") &&
		!is_valid_identifier(L"1var") &&
		is_valid_identifier(L"var1") &&
		is_valid_identifier(L"x1") &&
		is_valid_identifier(L"x_") &&
		!is_valid_identifier(L"_x")
	);
});

TEST(is_valid_identifier_token, {
	token_t *t=tokenize(L"x");
	classify_tokens(t);

	const bool pass=(t->token_type==TOKEN_IDENTIFIER);

	free(t);
	return pass;
});

TEST(new_identifier_clip_trailing_colon, {
	const wchar_t *code=L"x: int = 0";
	token_t *t=tokenize(code);
	classify_tokens(t);

	const bool pass=(
		t->token_type==TOKEN_NEW_IDENTIFIER &&
		t->end==(code + 1)
	);

	free(t);
	return pass;
});

TEST(identifier_cannot_be_type, {
	make_default_types();
	const wchar_t *code=L"int: int = 0";
	token_t *t=tokenize(code);
	classify_tokens(t);

	const bool pass=(
		t->token_type==TOKEN_UNKNOWN &&
		t->end==(code + 4)
	);

	free_types();
	free(t);
	return pass;
});

TEST(identifier_cannot_be_keyword, {
	make_default_types();
	const wchar_t *code=L"mut: int = 0";
	token_t *t=tokenize(code);
	classify_tokens(t);

	const bool pass=(
		t->token_type==TOKEN_UNKNOWN &&
		t->end==(code + 4)
	);

	free_types();
	free(t);
	return pass;
});

TEST(token_classifier, {
	const wchar_t *code=L"[ ]";
	token_t *t=tokenize(code);
	classify_tokens(t);

	const bool pass=(
		t->token_type==TOKEN_BRACKET_OPEN &&
		t->next->token_type==TOKEN_BRACKET_CLOSE
	);

	free(t->next);
	free(t);

	return pass;
});

void classifier_test_self(bool *pass) {
	tests_t tests={
		test_is_type_token,
		test_is_keyword_str,
		test_is_function_token,
		test_is_constant_integer,
		test_is_constant_float,
		test_is_constant_bool,
		test_is_constant_char,
		test_is_constant_str,
		test_bracket_token_open,
		test_bracket_token_close,
		test_token_mut_kw,
		test_token_return_kw,
		test_token_or_kw,
		test_token_and_kw,
		test_token_not_kw,
		test_token_equal_oper,
		test_token_plus_oper,
		test_token_equal_equal_oper,
		test_token_not_equal_oper,
		test_token_auto_equal_oper,
		test_token_type,
		test_token_function,
		test_token_unknown,
		test_token_integer_constant,
		test_token_float_constant,
		test_token_bool_constant,
		test_token_char_constant,
		test_token_str_constant,
		test_is_valid_identifier,
		test_is_valid_identifier_token,
		test_new_identifier_clip_trailing_colon,
		test_identifier_cannot_be_type,
		test_identifier_cannot_be_keyword,
		test_token_classifier,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
