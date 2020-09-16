#include <stdbool.h>

#include "skull/common/str.h"
#include "skull/parse/tokenize.h"

#include "test/skull/parse/tokenize.h"
#include "test/testing.h"

TEST(is_whitespace, {
	return (
		is_whitespace(' ') &&
		is_whitespace('\t') &&
		is_whitespace('\r') &&
		is_whitespace('\n') &&
		!is_whitespace('\v') &&
		!is_whitespace('A')
	);
})

TEST(is_quote, {
	return (
		is_quote('\"') &&
		is_quote('\'') &&
		!is_quote(U'字') &&
		!is_quote('A')
	);
})

TEST(tokenize_single_token, {
	const char32_t *code1=U"token";
	const char32_t *code2=U"token字";
	Token *t1=tokenize(code1);
	Token *t2=tokenize(code2);

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
	const char32_t *code=U"";
	Token *t=tokenize(code);

	const bool pass=(
		t->begin==code &&
		t->end==code
	);

	free(t);

	return pass;
})

TEST(whitespace_between_tokens, {
	const char32_t *code=U"token1\r\t token2";
	Token *t=tokenize(code);

	const bool pass=(
		t->begin==code &&
		t->end==(code + 6) &&
		t->next->begin==(code + 9) &&
		t->next->end==(code + 15)
	);

	free(t->next);
	free(t);

	return pass;
})

TEST(whitespace_at_eol_ignored, {
	const char32_t *code=U"token   ";
	Token *t=tokenize(code);

	const bool pass=(
		t->begin==code &&
		t->end==(code + 5)
	);

	free(t);

	return pass;
})

TEST(whitespace_inside_double_quotes_respected, {
	const char32_t *code=U"\"this is a single token\"";
	Token *t=tokenize(code);

	const bool pass=(
		!t->next &&
		t->begin==code &&
		t->end==(code + 24)
	);

	free(t);

	return pass;
})

TEST(whitespace_inside_single_quotes_respected, {
	const char32_t *code=U"'this is a single token'";
	Token *t=tokenize(code);

	const bool pass=(
		!t->next &&
		t->begin==code &&
		t->end==(code + 24)
	);

	free(t);

	return pass;
})

TEST(brackets_always_make_their_own_token, {
	const char32_t *code=U"left[]right";
	Token *t=tokenize(code);

	const bool pass=(
		t->begin==code &&
		t->end==(code + 4) &&
		t->next &&
		t->next->begin==(code + 4) &&
		t->next->end==(code + 5) &&
		t->next->next &&
		t->next->next->begin==(code + 5) &&
		t->next->next->end==(code + 6) &&
		t->next->next->next &&
		t->next->next->next->begin==(code + 6) &&
		t->next->next->next->end==(code + 11)
	);

	free(t);

	return pass;
})

TEST(newlines_always_make_their_own_token, {
	const char32_t *code=U"left\nright";
	Token *t=tokenize(code);

	const bool pass=(
		t->begin==code &&
		t->end==(code + 4) &&
		t->next &&
		t->next->begin==(code + 4) &&
		t->next->end==(code + 5) &&
		t->next->next &&
		t->next->next->begin==(code + 5) &&
		t->next->next->end==(code + 10)
	);

	free(t);

	return pass;
})

TEST(free_tokens, {
	const char32_t *code=U"token token token";
	Token *t=tokenize(code);

	free_tokens(t);

	return true;
})

TEST(token_len, {
	Token *token=tokenize(U"token");

	return token_len(token)==5;
})

TEST(token_cmp, {
	const char32_t *code=U"data";
	Token *token=tokenize(code);

	const bool pass=(
		token_cmp(U"data", token) &&
		!token_cmp(U"not_data", token)
	);

	free(token);

	return pass;
})

TEST(token_cmp_match_exact_strings_only, {
	Token *token1=tokenize(U"data");
	Token *token2=tokenize(U"dat");
	Token *token3=tokenize(U"da");
	Token *token4=tokenize(U"d");

	const bool pass=(
		token_cmp(U"data", token1) &&
		!token_cmp(U"data", token2) &&
		!token_cmp(U"data", token3) &&
		!token_cmp(U"data", token4)
	);

	free(token1);
	free(token2);
	free(token3);
	free(token4);
	return pass;
})

TEST(token_str, {
	const char32_t *code=U"left right";
	Token *token=tokenize(code);
	char32_t *buf=token_str(token);

	const bool pass=c32scmp(buf, U"left");

	free(buf);
	free_tokens(token);

	return pass;
})

TEST(tokenize_comment, {
	const char32_t *code=U"# this is a comment";
	Token *token=tokenize(code);
	char32_t *buf=token_str(token);

	const bool pass=c32scmp(buf, code);

	free(buf);
	free_tokens(token);

	return pass;
})

TEST(tokenize_trailing_comment, {
	const char32_t *code=U"stuff # this is a comment";
	Token *token=tokenize(code);
	char32_t *buf1=token_str(token);
	char32_t *buf2=token_str(token->next);

	const bool pass=(
		c32scmp(buf1, U"stuff") &&
		c32scmp(buf2, U"# this is a comment")
	);

	free(buf1);
	free(buf2);
	free_tokens(token);

	return pass;
})

TEST(make_token, {
	Token *token=make_token();

	const bool pass=(
		!token->begin &&
		!token->end &&
		!token->token_type &&
		!token->next
	);

	free(token);

	return pass;
})

TEST_SELF(tokenizer,
	test_is_whitespace,
	test_is_quote,
	test_tokenize_single_token,
	test_tokenize_no_tokens,
	test_whitespace_between_tokens,
	test_whitespace_at_eol_ignored,
	test_whitespace_inside_double_quotes_respected,
	test_whitespace_inside_single_quotes_respected,
	test_brackets_always_make_their_own_token,
	test_newlines_always_make_their_own_token,
	test_free_tokens,
	test_token_len,
	test_token_cmp,
	test_token_cmp_match_exact_strings_only,
	test_token_str,
	test_tokenize_comment,
	test_tokenize_trailing_comment,
	test_make_token
)
