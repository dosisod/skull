#include <stdbool.h>

#include "../src/parse/tokenize.h"
#include "../test/testing.h"

bool test_is_whitespace() {
	return is_whitespace(' ') && !is_whitespace('A');
}

bool test_is_quote() {
	return is_quote('\"') && is_quote('\'') && !is_quote('A');
}

bool test_tokenize_single_token() {
	token_t *t=tokenize("token");

	bool pass=(t->start==0 && t->end==5);
	free(t);

	return pass;
}

bool test_whitespace_between_tokens() {
	token_t *t=tokenize("token1\r\n\t token2");

	bool pass=(
		t->start==0 && t->end==6 &&
		t->next->start==10 && t->next->end==16
	);

	free(t->next);
	free(t);

	return pass;
}

bool test_whitespace_at_eol_ignored() {
	token_t *t=tokenize("token   ");

	bool pass=(t->start==0 && t->end==5);
	free(t);

	return pass;
}

bool test_whitespace_inside_double_quotes_respected() {
	token_t *t=tokenize("\"this is a single token\"");

	bool pass=(t->next==NULL && t->start==0 && t->end==24);
	free(t);

	return pass;
}

bool test_whitespace_inside_single_quotes_respected() {
	token_t *t=tokenize("'this is a single token'");

	bool pass=(t->next==NULL && t->start==0 && t->end==24);
	free(t);

	return pass;
}

bool test_free_tokens() {
	const char *code="token token token";
	token_t *t=tokenize(code);

	token_t *token2=t->next;
	token_t *token3=t->next->next;

	free_tokens(t);

	return (
		t->next==NULL &&
		token2->next==NULL &&
		token3->next==NULL
	);
}

bool test_token_len() {
	token_t *token=tokenize("token");

	return token_len(token)==5;
}

void tokenizer_test_self(bool *pass) {
	tests_t tests={
		test_is_whitespace,
		test_is_quote,
		test_tokenize_single_token,
		test_whitespace_between_tokens,
		test_whitespace_at_eol_ignored,
		test_whitespace_inside_double_quotes_respected,
		test_whitespace_inside_single_quotes_respected,
		test_free_tokens,
		test_token_len,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
