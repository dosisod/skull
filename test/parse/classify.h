#include <stdbool.h>

#include "../src/parse/classify.h"
#include "../test/testing.h"

bool test_is_keyword_token() {
	const char *code="if else not_keyword";
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
	const char *code="x = 10 / 2";
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
	const char *code="i32 not_a_type";
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
	const char *code1="main[]";
	const char *code2="function[i321";
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
	const char *code1="param_name]";
	const char *code2="param_name,";
	token_t *token1=tokenize(code1);
	token_t *token2=tokenize(code2);

	bool pass=(
		is_function_param_token(token1, code1) &&
		is_function_param_token(token2, code2)
	);

	free(token1);
	free(token2);

	return pass;
}

bool test_is_constant_integer() {
	return (
		is_constant_integer("123") &&
		is_constant_integer("-123") &&
		is_constant_integer("0x123") &&
		is_constant_integer("0x0123456789ABCDEF") &&
		is_constant_integer("0xabcdef") &&
		is_constant_integer("0b10101") &&
		!is_constant_integer("0b") &&
		!is_constant_integer("0x") &&
		!is_constant_integer("-") &&
		!is_constant_integer("123aaa") &&
		!is_constant_integer("-123aaa") &&
		!is_constant_integer("")
	);

	regfree(&int_regex);
	int_regex_compiled=false;
}

bool test_is_constant_float() {
	return (
		is_constant_float("123.0") &&
		is_constant_float("-123.0") &&
		is_constant_float("0.0") &&
		!is_constant_float("123") &&
		!is_constant_float("123.") &&
		!is_constant_float(".123") &&
		!is_constant_float("123aaa")
	);
}

bool test_is_constant_bool() {
	return (
		is_constant_bool("true") &&
		is_constant_bool("false") &&
		!is_constant_bool("not_bool")
	);
}

bool test_bracket_token_open() {
	const char *code="[";
	token_t *t=tokenize(code);
	classify_token(t, code);

	bool pass=(t->token_type==TOKEN_BRACKET_OPEN);
	free(t);

	return pass;
}

bool test_bracket_token_close() {
	const char *code="]";
	token_t *t=tokenize(code);
	classify_token(t, code);

	bool pass=(t->token_type==TOKEN_BRACKET_CLOSE);
	free(t);

	return pass;
}

bool test_token_keyword() {
	const char *code="if else return";
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
	const char *code="x = 10 / 2";
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
	const char *code="i32 not_a_type";
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
	const char *code="garbage_value";
	token_t *t=tokenize(code);
	classify_token(t, code);

	bool pass=(t->token_type==TOKEN_UNKNOWN);
	free(t);

	return pass;
}

bool test_token_function_no_params() {
	const char *code="main[]";
	token_t *t=tokenize(code);
	classify_tokens(t, code);

	bool pass=(t->token_type==TOKEN_FUNCTION);
	free(t);

	return pass;
}

bool test_token_function_param() {
	const char *code="param_name]";
	token_t *t=tokenize(code);
	classify_tokens(t, code);

	bool pass=(t->token_type==TOKEN_FUNCTION_PARAM);
	free(t);

	return pass;
}

bool test_token_integer_constant() {
	const char *code="1234";
	token_t *t=tokenize(code);
	classify_tokens(t, code);

	bool pass=(t->token_type==TOKEN_INT_CONST);
	free(t);

	return pass;
}

bool test_token_float_constant() {
	const char *code="123.0";
	token_t *t=tokenize(code);
	classify_tokens(t, code);

	bool pass=(t->token_type==TOKEN_FLOAT_CONST);
	free(t);

	return pass;
}

bool test_token_bool_constant() {
	const char *code="true";
	token_t *t=tokenize(code);
	classify_tokens(t, code);

	bool pass=(t->token_type==TOKEN_BOOL_CONST);
	free(t);

	return pass;
}

bool test_token_classifier() {
	const char *code="[ ]";
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
		test_token_classifier,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
