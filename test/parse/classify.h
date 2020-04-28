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
		test_bracket_token_open,
		test_bracket_token_close,
		test_token_keyword,
		test_token_operator,
		test_token_type,
		test_token_unknown,
		test_token_classifier,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
