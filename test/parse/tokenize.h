#include <stdbool.h>

#include "../../src/parse/tokenize.h"
#include "../../test/testing.h"

bool test_is_whitespace() {
	return (
		is_whitespace(' ') &&
		!is_whitespace('A') &&
		!is_whitespace(L'字')
	);
}

bool test_is_quote() {
	return (
		is_quote('\"') &&
		is_quote('\'') &&
		!is_quote(L'字') &&
		!is_quote('A')
	);
}

bool test_tokenize_single_token() {
	token_t *t1=tokenize(L"token");
	token_t *t2=tokenize(L"token字");

	const bool pass=(
		t1->start==0 && t1->end==5 &&
		t2->start==0 && t2->end==6
	);

	free(t1);
	free(t2);

	return pass;
}

bool test_tokenize_no_tokens() {
	token_t *t=tokenize(L"");

	const bool pass=(t->start==0 && t->end==0);
	free(t);

	return pass;
}

bool test_whitespace_between_tokens() {
	token_t *t=tokenize(L"token1\r\n\t token2");

	const bool pass=(
		t->start==0 && t->end==6 &&
		t->next->start==10 && t->next->end==16
	);

	free(t->next);
	free(t);

	return pass;
}

bool test_whitespace_at_eol_ignored() {
	token_t *t=tokenize(L"token   ");

	const bool pass=(t->start==0 && t->end==5);
	free(t);

	return pass;
}

bool test_whitespace_inside_double_quotes_respected() {
	token_t *t=tokenize(L"\"this is a single token\"");

	const bool pass=(t->next==NULL && t->start==0 && t->end==24);
	free(t);

	return pass;
}

bool test_whitespace_inside_single_quotes_respected() {
	token_t *t=tokenize(L"'this is a single token'");

	const bool pass=(t->next==NULL && t->start==0 && t->end==24);
	free(t);

	return pass;
}

bool test_free_tokens() {
	const wchar_t *code=L"token token token";
	token_t *t=tokenize(code);

	free_tokens(t);

	return true;
}

bool test_token_len() {
	token_t *token=tokenize(L"token");

	return token_len(token)==5;
}

bool test_token_cmp() {
	const wchar_t *code=L"data";
	token_t *token=tokenize(code);

	const bool pass=(
		token_cmp(L"data", token, code) &&
		!token_cmp(L"not_data", token, code)
	);

	free(token);

	return pass;
}

bool test_make_token() {
	token_t *token=make_token();

	const bool pass=(
		token->start==(size_t)-1 &&
		token->end==(size_t)-1 &&
		token->token_type==0 &&
		token->next==NULL
	);

	free(token);

	return pass;
}

void tokenizer_test_self(bool *pass) {
	tests_t tests={
		test_is_whitespace,
		test_is_quote,
		test_tokenize_single_token,
		test_tokenize_no_tokens,
		test_whitespace_between_tokens,
		test_whitespace_at_eol_ignored,
		test_whitespace_inside_double_quotes_respected,
		test_whitespace_inside_single_quotes_respected,
		test_free_tokens,
		test_token_len,
		test_token_cmp,
		test_make_token,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
