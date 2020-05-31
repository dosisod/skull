#include <stdbool.h>

#include "../../src/parse/classify.h"
#include "../../test/testing.h"

bool test_is_keyword_token(void) {
	const wchar_t *code=L"if else not_keyword";
	token_t *token=tokenize(code);

	if (token->next==NULL || token->next->next==NULL) {
		return false;
	}

	const bool pass=(
		is_keyword_token(token) &&
		is_keyword_token(token->next) &&
		!is_keyword_token(token->next->next)
	);

	free_tokens(token);

	return pass;
}

bool test_is_operator_token(void) {
	const wchar_t *code=L"x = 10 / 2";
	token_t *token=tokenize(code);

	if (
		token->next==NULL ||
		token->next->next==NULL ||
		token->next->next->next==NULL ||
		token->next->next->next->next==NULL
	) {
		return false;
	}

	const bool pass=(
		!is_operator_token(token) &&
		is_operator_token(token->next) &&
		!is_operator_token(token->next->next) &&
		is_operator_token(token->next->next->next) &&
		!is_operator_token(token->next->next->next->next)
	);

	free_tokens(token);

	return pass;
}

bool test_is_type_token(void) {
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
}

bool test_is_function_token(void) {
	const wchar_t *code=L"main[] ->";
	token_t *token=tokenize(code);

	const bool pass=(
		!is_function_token(token) &&
		is_function_token(token->next->next->next)
	);

	free_tokens(token);

	return pass;
}

bool test_is_constant_integer(void) {
	return (
		is_constant_integer(L"123") &&
		is_constant_integer(L"-123") &&
		is_constant_integer(L"0x123") &&
		is_constant_integer(L"0x0123456789ABCDEF") &&
		is_constant_integer(L"0xabcdef") &&
		is_constant_integer(L"0b10101") &&
		!is_constant_integer(L"0b") &&
		!is_constant_integer(L"0x") &&
		!is_constant_integer(L"-") &&
		!is_constant_integer(L"123aaa") &&
		!is_constant_integer(L"-123aaa") &&
		!is_constant_integer(L"")
	);
}

bool test_is_constant_float(void) {
	return (
		is_constant_float(L"123.0") &&
		is_constant_float(L"-123.0") &&
		is_constant_float(L"0.0") &&
		!is_constant_float(L"123") &&
		!is_constant_float(L"123.") &&
		!is_constant_float(L".123") &&
		!is_constant_float(L"123aaa")
	);
}

bool test_is_constant_bool(void) {
	return (
		is_constant_bool(L"true") &&
		is_constant_bool(L"false") &&
		!is_constant_bool(L"not_bool")
	);
}

bool test_is_constant_char(void) {
	return (
		is_constant_char(L"'x'") &&
		!is_constant_char(L"'x '") &&
		!is_constant_char(L"''")
	);
}

bool test_is_constant_str(void) {
	return (
		is_constant_str(L"\"\"") &&
		is_constant_str(L"\"x\"") &&
		is_constant_str(L"\"xyz\"") &&
		!is_constant_str(L"\"bad") &&
		!is_constant_str(L"bad\"")
	);
}

bool test_bracket_token_open(void) {
	const wchar_t *code=L"[";
	token_t *t=tokenize(code);
	classify_token(t);

	const bool pass=(t->token_type==TOKEN_BRACKET_OPEN);
	free(t);

	return pass;
}

bool test_bracket_token_close(void) {
	const wchar_t *code=L"]";
	token_t *t=tokenize(code);
	classify_token(t);

	const bool pass=(t->token_type==TOKEN_BRACKET_CLOSE);
	free(t);

	return pass;
}

bool test_token_keyword(void) {
	const wchar_t *code=L"if else return";
	token_t *t=tokenize(code);
	classify_tokens(t);

	const bool pass=(
		t->token_type==TOKEN_KEYWORD &&
		t->next->token_type==TOKEN_KEYWORD &&
		t->next->next->token_type==TOKEN_KEYWORD
	);

	free_tokens(t);

	return pass;
}

bool test_token_operator(void) {
	const wchar_t *code=L"x = 10 / 2";
	token_t *t=tokenize(code);
	classify_tokens(t);

	const bool pass=(
		t->token_type!=TOKEN_OPERATOR &&
		t->next->token_type==TOKEN_OPERATOR &&
		t->next->next->token_type!=TOKEN_OPERATOR &&
		t->next->next->next->token_type==TOKEN_OPERATOR &&
		t->next->next->next->next->token_type!=TOKEN_OPERATOR
	);

	free_tokens(t);

	return pass;
}

bool test_token_type(void) {
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
}

bool test_token_function(void) {
	const wchar_t *code=L"main[] ->";
	token_t *t=tokenize(code);
	classify_tokens(t);

	const bool pass=(
		t->token_type!=TOKEN_FUNCTION &&
		t->next->next->next->token_type==TOKEN_FUNCTION
	);

	free_tokens(t);

	return pass;
}

bool test_token_unknown(void) {
	const wchar_t *code=L"123garbage_value";
	token_t *t=tokenize(code);
	classify_token(t);

	const bool pass=(t->token_type==TOKEN_UNKNOWN);
	free(t);

	return pass;
}

bool test_token_integer_constant(void) {
	const wchar_t *code=L"1234";
	token_t *t=tokenize(code);
	classify_tokens(t);

	const bool pass=(t->token_type==TOKEN_INT_CONST);
	free(t);

	return pass;
}

bool test_token_float_constant(void) {
	const wchar_t *code=L"123.0";
	token_t *t=tokenize(code);
	classify_tokens(t);

	const bool pass=(t->token_type==TOKEN_FLOAT_CONST);
	free(t);

	return pass;
}

bool test_token_bool_constant(void) {
	const wchar_t *code=L"true";
	token_t *t=tokenize(code);
	classify_tokens(t);

	const bool pass=(t->token_type==TOKEN_BOOL_CONST);
	free(t);

	return pass;
}

bool test_token_char_constant(void) {
	const wchar_t *code=L"'x'";
	token_t *t=tokenize(code);
	classify_tokens(t);

	const bool pass=(t->token_type==TOKEN_CHAR_CONST);
	free(t);

	return pass;
}

bool test_token_str_constant(void) {
	const wchar_t *code=L"\"xyz\"";
	token_t *t=tokenize(code);
	classify_tokens(t);

	const bool pass=(t->token_type==TOKEN_STR_CONST);
	free(t);

	return pass;
}

bool test_is_valid_identifier(void) {
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
}

bool test_is_valid_identifier_token(void) {
	token_t *t=tokenize(L"x");
	classify_tokens(t);

	const bool pass=(t->token_type==TOKEN_IDENTIFIER);

	free(t);
	return pass;
}

bool test_identifier_clip_trailing_colon(void) {
	const wchar_t *code=L"x: int = 0";
	token_t *t=tokenize(code);
	classify_tokens(t);

	const bool pass=(
		t->token_type==TOKEN_IDENTIFIER &&
		t->end==(code + 1)
	);

	free(t);
	return pass;
}

bool test_token_classifier(void) {
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
}

void classifier_test_self(bool *pass) {
	tests_t tests={
		test_is_keyword_token,
		test_is_operator_token,
		test_is_type_token,
		test_is_function_token,
		test_is_constant_integer,
		test_is_constant_float,
		test_is_constant_bool,
		test_is_constant_char,
		test_is_constant_str,
		test_bracket_token_open,
		test_bracket_token_close,
		test_token_keyword,
		test_token_operator,
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
		test_identifier_clip_trailing_colon,
		test_token_classifier,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
