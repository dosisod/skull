#include <stdbool.h>

#include "../src/parse/classify.h"
#include "../test/testing.h"

bool test_is_keyword_token() {
	const wchar_t *code=L"if else not_keyword";
	token_t *token=tokenize(code);

	bool pass=(
		is_keyword_token(token, code) &&
		is_keyword_token(token->next, code) &&
		!is_keyword_token(token->next->next, code)
	);

	free_tokens(token);

	return pass;
}

bool test_is_operator_token() {
	const wchar_t *code=L"x = 10 / 2";
	token_t *token=tokenize(code);

	bool pass=(
		!is_operator_token(token, code) &&
		is_operator_token(token->next, code) &&
		!is_operator_token(token->next->next, code) &&
		is_operator_token(token->next->next->next, code) &&
		!is_operator_token(token->next->next->next->next, code)
	);

	free_tokens(token);

	return pass;
}

bool test_is_type_token() {
	const wchar_t *code=L"i32 not_a_type";
	token_t *token=tokenize(code);

	make_default_types();

	bool pass=(
		is_type_token(token, code) &&
		!is_type_token(token->next, code)
	);

	free_tokens(token);

	return pass;
}

bool test_is_function_token() {
	const wchar_t *code1=L"main[]";
	const wchar_t *code2=L"function[i321";
	token_t *token1=tokenize(code1);
	token_t *token2=tokenize(code2);

	bool pass=(
		is_function_token(token1, code1) &&
		is_function_token(token2, code2)
	);

	free(token1);
	free(token2);

	return pass;
}

bool test_is_function_param_token() {
	const wchar_t *code1=L"param_name]";
	const wchar_t *code2=L"param_name,";
	const wchar_t *code3=L"x]";
	const wchar_t *code4=L"x,";
	token_t *token1=tokenize(code1);
	token_t *token2=tokenize(code2);
	token_t *token3=tokenize(code3);
	token_t *token4=tokenize(code4);

	bool pass=(
		is_function_param_token(token1, code1) &&
		is_function_param_token(token2, code2) &&
		is_function_param_token(token3, code3) &&
		is_function_param_token(token4, code4)
	);

	free(token1);
	free(token2);

	return pass;
}

bool test_is_constant_integer() {
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

	regfree(&int_regex);
	int_regex_compiled=false;
}

bool test_is_constant_float() {
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

bool test_is_constant_bool() {
	return (
		is_constant_bool(L"true") &&
		is_constant_bool(L"false") &&
		!is_constant_bool(L"not_bool")
	);
}

bool test_is_constant_char() {
	return (
		is_constant_char(L"'x'") &&
		!is_constant_char(L"'x '") &&
		!is_constant_char(L"''")
	);
}

bool test_is_constant_str() {
	return (
		is_constant_str(L"\"\"") &&
		is_constant_str(L"\"x\"") &&
		is_constant_str(L"\"xyz\"") &&
		!is_constant_str(L"\"bad") &&
		!is_constant_str(L"bad\"")
	);
}

bool test_bracket_token_open() {
	const wchar_t *code=L"[";
	token_t *t=tokenize(code);
	classify_token(t, code);

	bool pass=(t->token_type==TOKEN_BRACKET_OPEN);
	free(t);

	return pass;
}

bool test_bracket_token_close() {
	const wchar_t *code=L"]";
	token_t *t=tokenize(code);
	classify_token(t, code);

	bool pass=(t->token_type==TOKEN_BRACKET_CLOSE);
	free(t);

	return pass;
}

bool test_token_keyword() {
	const wchar_t *code=L"if else return";
	token_t *t=tokenize(code);
	classify_tokens(t, code);

	bool pass=(
		t->token_type==TOKEN_KEYWORD &&
		t->next->token_type==TOKEN_KEYWORD &&
		t->next->next->token_type==TOKEN_KEYWORD
	);

	free_tokens(t);

	return pass;
}

bool test_token_operator() {
	const wchar_t *code=L"x = 10 / 2";
	token_t *t=tokenize(code);
	classify_tokens(t, code);

	bool pass=(
		t->token_type!=TOKEN_OPERATOR &&
		t->next->token_type==TOKEN_OPERATOR &&
		t->next->next->token_type!=TOKEN_OPERATOR &&
		t->next->next->next->token_type==TOKEN_OPERATOR &&
		t->next->next->next->next->token_type!=TOKEN_OPERATOR
	);

	free_tokens(t);

	return pass;
}

bool test_token_type() {
	const wchar_t *code=L"i32 not_a_type";
	token_t *t=tokenize(code);
	classify_tokens(t, code);

	bool pass=(
		t->token_type==TOKEN_TYPE &&
		t->next->token_type!=TOKEN_TYPE
	);

	free_tokens(t);

	return pass;
}

bool test_token_unknown() {
	const wchar_t *code=L"garbage_value";
	token_t *t=tokenize(code);
	classify_token(t, code);

	bool pass=(t->token_type==TOKEN_UNKNOWN);
	free(t);

	return pass;
}

bool test_token_function_no_params() {
	const wchar_t *code=L"main[]";
	token_t *t=tokenize(code);
	classify_tokens(t, code);

	bool pass=(t->token_type==TOKEN_FUNCTION);
	free(t);

	return pass;
}

bool test_token_function_param() {
	const wchar_t *code=L"param_name]";
	token_t *t=tokenize(code);
	classify_tokens(t, code);

	bool pass=(t->token_type==TOKEN_FUNCTION_PARAM);
	free(t);

	return pass;
}

bool test_token_integer_constant() {
	const wchar_t *code=L"1234";
	token_t *t=tokenize(code);
	classify_tokens(t, code);

	bool pass=(t->token_type==TOKEN_INT_CONST);
	free(t);

	return pass;
}

bool test_token_float_constant() {
	const wchar_t *code=L"123.0";
	token_t *t=tokenize(code);
	classify_tokens(t, code);

	bool pass=(t->token_type==TOKEN_FLOAT_CONST);
	free(t);

	return pass;
}

bool test_token_bool_constant() {
	const wchar_t *code=L"true";
	token_t *t=tokenize(code);
	classify_tokens(t, code);

	bool pass=(t->token_type==TOKEN_BOOL_CONST);
	free(t);

	return pass;
}

bool test_token_char_constant() {
	const wchar_t *code=L"'x'";
	token_t *t=tokenize(code);
	classify_tokens(t, code);

	bool pass=(t->token_type==TOKEN_CHAR_CONST);
	free(t);

	return pass;
}

bool test_token_str_constant() {
	const wchar_t *code=L"\"xyz\"";
	token_t *t=tokenize(code);
	classify_tokens(t, code);

	bool pass=(t->token_type==TOKEN_STR_CONST);
	free(t);

	return pass;
}

bool test_token_classifier() {
	const wchar_t *code=L"[ ]";
	token_t *t=tokenize(code);
	classify_tokens(t, code);

	bool pass=(
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
		test_is_function_param_token,
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
		test_token_unknown,
		test_token_function_no_params,
		test_token_function_param,
		test_token_integer_constant,
		test_token_float_constant,
		test_token_bool_constant,
		test_token_char_constant,
		test_token_str_constant,
		test_token_classifier,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
