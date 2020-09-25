#include <stdbool.h>

#include "skull/common/str.h"
#include "skull/parse/tokenize.h"

#include "test/skull/parse/tokenize.h"
#include "test/testing.h"

TEST(is_whitespace, {
	ASSERT_TRUTHY(is_whitespace(' '));
	ASSERT_TRUTHY(is_whitespace('\t'));
	ASSERT_TRUTHY(is_whitespace('\r'));
	ASSERT_TRUTHY(is_whitespace('\n'));
	ASSERT_FALSEY(is_whitespace('\v'));
	ASSERT_FALSEY(is_whitespace('A'));
})

TEST(is_quote, {
	ASSERT_TRUTHY(is_quote('\"'));
	ASSERT_TRUTHY(is_quote('\''));
	ASSERT_FALSEY(is_quote(U'字'));
	ASSERT_FALSEY(is_quote('A'));
})

TEST(tokenize_single_token, {
	const char32_t *code1 = U"token";
	const char32_t *code2 = U"token字";
	Token *t1 = tokenize(code1);
	Token *t2 = tokenize(code2);

	ASSERT_EQUAL(t1->begin, code1);
	ASSERT_EQUAL(t1->end, code1 + 5);
	ASSERT_EQUAL(t2->begin, code2);
	ASSERT_EQUAL(t2->end, code2 + 6);

	free(t1);
	free(t2);
})

TEST(tokenize_no_tokens, {
	const char32_t *code = U"";
	Token *t = tokenize(code);

	ASSERT_EQUAL(t->begin, code);
	ASSERT_EQUAL(t->end, code);

	free(t);
})

TEST(whitespace_between_tokens, {
	const char32_t *code = U"token1\r\t token2";
	Token *t = tokenize(code);

	ASSERT_EQUAL(t->begin, code);
	ASSERT_EQUAL(t->end, code + 6);
	ASSERT_EQUAL(t->next->begin, code + 9);
	ASSERT_EQUAL(t->next->end, code + 15);

	free(t->next);
	free(t);
})

TEST(whitespace_at_eol_ignored, {
	const char32_t *code = U"token   ";
	Token *t = tokenize(code);

	ASSERT_EQUAL(t->begin, code);
	ASSERT_EQUAL(t->end, code + 5);

	free(t);
})

TEST(whitespace_inside_double_quotes_respected, {
	const char32_t *code = U"\"this is a single token\"";
	Token *t = tokenize(code);

	ASSERT_FALSEY(t->next);
	ASSERT_EQUAL(t->begin, code);
	ASSERT_EQUAL(t->end, code + 24);

	free(t);
})

TEST(whitespace_inside_single_quotes_respected, {
	const char32_t *code = U"'this is a single token'";
	Token *t = tokenize(code);

	ASSERT_FALSEY(t->next);
	ASSERT_EQUAL(t->begin, code);
	ASSERT_EQUAL(t->end, code + 24);

	free(t);
})

TEST(brackets_always_make_their_own_token, {
	const char32_t *code = U"left[]right";
	Token *t = tokenize(code);

	ASSERT_EQUAL(t->begin, code);
	ASSERT_EQUAL(t->end, code + 4);
	ASSERT_TRUTHY(t->next);
	ASSERT_EQUAL(t->next->begin, code + 4);
	ASSERT_EQUAL(t->next->end, code + 5);
	ASSERT_TRUTHY(t->next->next);
	ASSERT_EQUAL(t->next->next->begin, code + 5);
	ASSERT_EQUAL(t->next->next->end, code + 6);
	ASSERT_TRUTHY(t->next->next->next);
	ASSERT_EQUAL(t->next->next->next->begin, code + 6);
	ASSERT_EQUAL(t->next->next->next->end, code + 11);

	free(t);
})

TEST(newlines_always_make_their_own_token, {
	const char32_t *code = U"left\nright";
	Token *t = tokenize(code);

	ASSERT_EQUAL(t->begin, code);
	ASSERT_EQUAL(t->end, code + 4);
	ASSERT_TRUTHY(t->next);
	ASSERT_EQUAL(t->next->begin, code + 4);
	ASSERT_EQUAL(t->next->end, code + 5);
	ASSERT_TRUTHY(t->next->next);
	ASSERT_EQUAL(t->next->next->begin, code + 5);
	ASSERT_EQUAL(t->next->next->end, code + 10);

	free(t);
})

TEST(free_tokens, {
	free_tokens(tokenize(U"token token token"));
})

TEST(token_len, {
	Token *token = tokenize(U"token");

	ASSERT_EQUAL(token_len(token), 5);

	free_tokens(token);
})

TEST(token_cmp, {
	Token *token = tokenize(U"data");

	ASSERT_TRUTHY(token_cmp(U"data", token));
	ASSERT_FALSEY(token_cmp(U"not_data", token));

	free(token);
})

TEST(token_cmp_match_exact_strings_only, {
	Token *token1 = tokenize(U"data");
	Token *token2 = tokenize(U"dat");
	Token *token3 = tokenize(U"da");
	Token *token4 = tokenize(U"d");

	ASSERT_TRUTHY(token_cmp(U"data", token1));
	ASSERT_FALSEY(token_cmp(U"data", token2));
	ASSERT_FALSEY(token_cmp(U"data", token3));
	ASSERT_FALSEY(token_cmp(U"data", token4));

	free(token1);
	free(token2);
	free(token3);
	free(token4);
})

TEST(token_str, {
	Token *token = tokenize(U"left right");
	char32_t *buf = token_str(token);

	ASSERT_TRUTHY(c32scmp(buf, U"left"));

	free(buf);
	free_tokens(token);
})

TEST(tokenize_comment, {
	const char32_t *code = U"# this is a comment";
	Token *token = tokenize(code);
	char32_t *buf = token_str(token);

	ASSERT_TRUTHY(c32scmp(buf, code));

	free(buf);
	free_tokens(token);
})

TEST(tokenize_trailing_comment, {
	Token *token = tokenize(U"stuff # this is a comment");
	char32_t *buf1 = token_str(token);
	char32_t *buf2 = token_str(token->next);

	ASSERT_TRUTHY(c32scmp(buf1, U"stuff"));
	ASSERT_TRUTHY(c32scmp(buf2, U"# this is a comment"));

	free(buf1);
	free(buf2);
	free_tokens(token);
})

TEST(make_token, {
	Token *token = make_token();

	ASSERT_FALSEY(token->begin);
	ASSERT_FALSEY(token->end);
	ASSERT_FALSEY(token->token_type);
	ASSERT_FALSEY(token->next);

	free(token);
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
