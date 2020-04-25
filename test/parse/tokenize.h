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

void tokenizer_test_self(bool *failed) {
	tests_t tests={
		test_tokenize_single_token,
		test_whitespace_between_tokens,
		test_whitespace_at_eol_ignored,
		NULL
	};

	run_many_tests(__FILE__, tests, failed);
}
