#include <stdbool.h>

#include "../src/parse/tokenize.h"
#include "../test/testing.h"

bool test_tokenize_single_token() {
	token_t *t=tokenize("token");

	bool pass=(t->start==0 && t->end==5);
	free(t);

	return !pass;
}

bool test_whitespace_between_tokens() {
	token_t *t=tokenize("token1\r\n\t token2");

	bool pass=(
		t->start==0 && t->end==6 &&
		t->next->start==10 && t->next->end==16
	);

	free(t->next);
	free(t);

	return !pass;
}

bool test_whitespace_at_eol_ignored() {
	token_t *t=tokenize("token   ");

	bool pass=(t->start==0 && t->end==5);
	free(t);

	return !pass;
}

bool test_bracket_token_open() {
	const char *code="[";
	token_t *t=tokenize(code);
	classify_token(t, code);

	bool pass=(t->token_type==TOKEN_BRACKET_OPEN);
	free(t);

	return !pass;
}

bool test_bracket_token_close() {
	const char *code="]";
	token_t *t=tokenize(code);
	classify_token(t, code);

	bool pass=(t->token_type==TOKEN_BRACKET_CLOSE);
	free(t);

	return !pass;
}

bool test_token_keyword() {
	const char *code="return";
	token_t *t=tokenize(code);
	classify_token(t, code);

	bool pass=(t->token_type==TOKEN_KEYWORD);
	free(t);

	return !pass;
}

bool test_token_unknown() {
	const char *code="garbage_value";
	token_t *t=tokenize(code);
	classify_token(t, code);

	bool pass=(t->token_type==TOKEN_UNKNOWN);
	free(t);

	return !pass;
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

	return !pass;
}

void tokenizer_test_self(bool *failed) {
	tests_t tests={
		test_tokenize_single_token,
		test_whitespace_between_tokens,
		test_whitespace_at_eol_ignored,
		test_bracket_token_open,
		test_bracket_token_close,
		test_token_keyword,
		test_token_unknown,
		test_token_classifier,
		NULL
	};

	run_many_tests(__FILE__, tests, failed);
}
