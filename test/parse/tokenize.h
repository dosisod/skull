#include <stdbool.h>

#include "../../src/parse/tokenize.h"
#include "../../test/testing.h"

TEST(is_whitespace, {
	return (
		is_whitespace(' ') &&
		!is_whitespace('A') &&
		!is_whitespace(L'字')
	);
})

TEST(is_quote, {
	return (
		is_quote('\"') &&
		is_quote('\'') &&
		!is_quote(L'字') &&
		!is_quote('A')
	);
})

TEST(tokenize_single_token, {
	const wchar_t *code1=L"token";
	const wchar_t *code2=L"token字";
	token_t *t1=tokenize(code1);
	token_t *t2=tokenize(code2);

	const bool pass=(
		t1->begin==code1 &&
		t1->end==(code1 + 5) &&
		t2->begin==code2 &&
		t2->end==(code2 + 6)
	);

	free(t1);
	free(t2);

	return pass;
})

TEST(tokenize_no_tokens, {
	const wchar_t *code=L"";
	token_t *t=tokenize(code);

	const bool pass=(
		t->begin==code &&
		t->end==code
	);

	free(t);

	return pass;
})

TEST(whitespace_between_tokens, {
	const wchar_t *code=L"token1\r\v\t token2";
	token_t *t=tokenize(code);

	const bool pass=(
		t->begin==&code[0] &&
		t->end==&code[6] &&
		t->next->begin==&code[10] &&
		t->next->end==&code[16]
	);

	free(t->next);
	free(t);

	return pass;
})

TEST(whitespace_at_eol_ignored, {
	const wchar_t *code=L"token   ";
	token_t *t=tokenize(code);

	const bool pass=(
		t->begin==&code[0] &&
		t->end==&code[5]
	);

	free(t);

	return pass;
})

TEST(whitespace_inside_double_quotes_respected, {
	const wchar_t *code=L"\"this is a single token\"";
	token_t *t=tokenize(code);

	const bool pass=(
		t->next==NULL &&
		t->begin==&code[0] &&
		t->end==&code[24]
	);

	free(t);

	return pass;
})

TEST(whitespace_inside_single_quotes_respected, {
	const wchar_t *code=L"'this is a single token'";
	token_t *t=tokenize(code);

	const bool pass=(
		t->next==NULL &&
		t->begin==&code[0] &&
		t->end==&code[24]
	);

	free(t);

	return pass;
})

TEST(brackets_always_make_their_own_token, {
	const wchar_t *code=L"left[]right";
	token_t *t=tokenize(code);

	const bool pass=(
		t->begin==code &&
		t->end==(code + 4) &&
		t->next!=NULL &&
		t->next->begin==(code + 4) &&
		t->next->end==(code + 5) &&
		t->next->next!=NULL &&
		t->next->next->begin==(code + 5) &&
		t->next->next->end==(code + 6) &&
		t->next->next->next!=NULL &&
		t->next->next->next->begin==(code + 6) &&
		t->next->next->next->end==(code + 11)
	);

	free(t);

	return pass;
})

TEST(comma_makes_their_own_token, {
	const wchar_t *code=L"left,right";
	token_t *t=tokenize(code);

	const bool pass=(
		t->next!=NULL &&
		t->next->begin==&code[4] &&
		t->next->end==&code[5]
	);

	free(t);

	return pass;
})

TEST(newlines_makes_their_own_token, {
	const wchar_t *code=L"left\nright";
	token_t *t=tokenize(code);

	const bool pass=(
		t->next!=NULL &&
		t->next->begin==&code[4] &&
		t->next->end==&code[5]
	);

	free(t);

	return pass;
})

TEST(free_tokens, {
	const wchar_t *code=L"token token token";
	token_t *t=tokenize(code);

	free_tokens(t);

	return true;
})

TEST(token_len, {
	token_t *token=tokenize(L"token");

	return token_len(token)==5;
})

TEST(token_cmp, {
	const wchar_t *code=L"data";
	token_t *token=tokenize(code);

	const bool pass=(
		token_cmp(L"data", token) &&
		!token_cmp(L"not_data", token)
	);

	free(token);

	return pass;
})

TEST(token_cmp_match_exact_strings_only, {
	token_t *token1=tokenize(L"data");
	token_t *token2=tokenize(L"dat");
	token_t *token3=tokenize(L"da");
	token_t *token4=tokenize(L"d");

	const bool pass=(
		token_cmp(L"data", token1) &&
		!token_cmp(L"data", token2) &&
		!token_cmp(L"data", token3) &&
		!token_cmp(L"data", token4)
	);

	free(token1);
	free(token2);
	free(token3);
	free(token4);
	return pass;
})

TEST(token_str, {
	const wchar_t *code=L"left right";
	token_t *token=tokenize(code);
	wchar_t *buf=token_str(token);

	const bool pass=(wcscmp(buf, L"left")==0);

	free(buf);
	free_tokens(token);

	return pass;
})

TEST(make_token, {
	token_t *token=make_token();

	const bool pass=(
		token->begin==NULL &&
		token->end==NULL &&
		token->token_type==0 &&
		token->next==NULL
	);

	free(token);

	return pass;
})

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
		test_brackets_always_make_their_own_token,
		test_comma_makes_their_own_token,
		test_newlines_makes_their_own_token,
		test_free_tokens,
		test_token_len,
		test_token_cmp,
		test_token_cmp_match_exact_strings_only,
		test_token_str,
		test_make_token,
		NULL
	};

	run_many_tests(__FILE__, tests, pass);
}
