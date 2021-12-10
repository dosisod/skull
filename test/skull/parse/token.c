#include <stdbool.h>
#include <stdlib.h>

#include "skull/common/errors.h"
#include "skull/common/str.h"
#include "skull/parse/token.h"

#include "test/skull/parse/token.h"
#include "test/testing.h"

static bool test_tokenize_single_token(void) {
	const char32_t *code1 = U"token";
	const char32_t *code2 = U"token字";
	Token *t1 = tokenize(code1);
	Token *t2 = tokenize(code2);

	ASSERT_EQUAL(t1->begin, code1);
	ASSERT_EQUAL(t1->end, code1 + 5);
	ASSERT_EQUAL(t2->begin, code2);
	ASSERT_EQUAL(t2->end, code2 + 6);

	free_tokens(t1);
	free_tokens(t2);

	PASS
}

static bool test_tokenize_no_tokens(void) {
	const char32_t *code = U"";
	Token *t = tokenize(code);

	ASSERT_FALSEY(t->begin);
	ASSERT_FALSEY(t->end);

	free_tokens(t);

	PASS
}

static bool test_whitespace_between_tokens(void) {
	const char32_t *code = U"token1\r\t token2";
	Token *t = tokenize(code);

	ASSERT_EQUAL(t->begin, code);
	ASSERT_EQUAL(t->end, code + 6);
	ASSERT_EQUAL(t->next->begin, code + 9);
	ASSERT_EQUAL(t->next->end, code + 15);

	free_tokens(t);

	PASS
}

static bool test_whitespace_at_eol_ignored(void) {
	const char32_t *code = U"token   ";
	Token *t = tokenize(code);

	ASSERT_EQUAL(t->begin, code);
	ASSERT_EQUAL(t->end, code + 5);

	free_tokens(t);

	PASS
}

static bool test_whitespace_inside_double_quotes_respected(void) {
	const char32_t *code = U"\"this is a single token\"";
	Token *t = tokenize(code);

	ASSERT_FALSEY(t->next);
	ASSERT_EQUAL(t->begin, code);
	ASSERT_EQUAL(t->end, code + 24);

	free_tokens(t);

	PASS
}

static bool test_whitespace_inside_single_quotes_respected(void) {
	const char32_t *code = U"'this is a single token'";
	Token *t = tokenize(code);

	ASSERT_FALSEY(t->next);
	ASSERT_EQUAL(t->begin, code);
	ASSERT_EQUAL(t->end, code + 24);

	free_tokens(t);

	PASS
}

static bool test_brackets_always_make_their_own_token(void) {
	const char32_t *code = U"left{}right";
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

	free_tokens(t);

	PASS
}

static bool test_newlines_always_make_their_own_token(void) {
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

	free_tokens(t);

	PASS
}

static bool test_token_len(void) {
	Token *token = tokenize(U"token");

	ASSERT_EQUAL(token_len(token), 5);

	free_tokens(token);

	PASS
}

static bool test_token_cmp(void) {
	Token *token = tokenize(U"data");

	ASSERT_TRUTHY(token_cmp(U"data", token));
	ASSERT_FALSEY(token_cmp(U"not_data", token));

	free_tokens(token);

	PASS
}

static bool test_token_cmp_match_exact_strings_only(void) {
	Token *token1 = tokenize(U"data");
	Token *token2 = tokenize(U"dat");
	Token *token3 = tokenize(U"da");
	Token *token4 = tokenize(U"d");

	ASSERT_TRUTHY(token_cmp(U"data", token1));
	ASSERT_FALSEY(token_cmp(U"data", token2));
	ASSERT_FALSEY(token_cmp(U"data", token3));
	ASSERT_FALSEY(token_cmp(U"data", token4));

	free_tokens(token1);
	free_tokens(token2);
	free_tokens(token3);
	free_tokens(token4);

	PASS
}

static bool test_token_to_string(void) {
	Token *token = tokenize(U"left right");
	char32_t *buf = token_to_string(token);

	ASSERT_TRUTHY(c32scmp(buf, U"left"));

	free(buf);
	free_tokens(token);

	PASS
}

static bool test_tokenize_comment(void) {
	#define comment1 U"# this is a comment"
	#define comment2 U"#\talso a comment"
	const char32_t *code = comment1 U"\n" comment2;

	Token *token = tokenize(code);
	char32_t *buf = token_to_string(token);
	char32_t *buf2 = token_to_string(token->next->next);

	ASSERT_TRUTHY(c32scmp(buf, comment1));
	ASSERT_TRUTHY(c32scmp(buf2, comment2));

	free(buf);
	free(buf2);
	free_tokens(token);

	PASS

	#undef comment1
	#undef comment2
}

static bool test_tokenize_trailing_comment(void) {
	Token *token = tokenize(U"stuff # this is a comment");
	char32_t *buf1 = token_to_string(token);
	char32_t *buf2 = token_to_string(token->next);

	ASSERT_TRUTHY(c32scmp(buf1, U"stuff"));
	ASSERT_TRUTHY(c32scmp(buf2, U"# this is a comment"));

	free(buf1);
	free(buf2);
	free_tokens(token);

	PASS
}

static bool test_make_token(void) {
	Token *token = make_token();

	ASSERT_FALSEY(token->begin);
	ASSERT_FALSEY(token->end);
	ASSERT_FALSEY(token->type);
	ASSERT_FALSEY(token->next);

	free_tokens(token);

	PASS
}

static bool test_tokenize_with_lines_and_columns(void) {
	const char32_t *code = U"token1\ntoken2";
	Token *t = tokenize(code);

	ASSERT_EQUAL(t->location.line, 1);
	ASSERT_EQUAL(t->location.column, 1);
	ASSERT_EQUAL(t->next->location.line, 1);
	ASSERT_EQUAL(t->next->location.column, 7);
	ASSERT_EQUAL(t->next->next->location.line, 2);
	ASSERT_EQUAL(t->next->next->location.column, 1);

	free_tokens(t);

	PASS
}

static bool test_nested_block_comment_fails(void) {
	free_errors();

	Token *t = tokenize(U"#{\n#{\n");

	ASSERT_FALSEY(t);
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 2 column 2: cannot put opening block comment in existing block comment\n"
	));

	PASS
}

static bool test_warn_bom(void) {
	free_errors();

	Token *t = tokenize(U"\xFEFF");

	ASSERT_TRUTHY(t);
	ASSERT_FALSEY(compare_errors("(null): Warning: BOM found\n"));

	free_tokens(t);
	PASS
}

static bool test_invalid_comment_start(void) {
	free_errors();

	Token *t = tokenize(U"#x");

	ASSERT_FALSEY(t);
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: invalid start of comment\n"
	));

	PASS
}

static bool test_check_for_missing_block_comment(void) {
	free_errors();

	Token *t = tokenize(U"#{");

	ASSERT_FALSEY(t);
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: expected closing \"#}\" for block comment\n"
	));

	PASS
}

static bool test_check_for_closing_single_quote(void) {
	free_errors();

	Token *t = tokenize(U"\'");

	ASSERT_FALSEY(t);
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: expected closing quote\n"
	));

	PASS
}

static bool test_check_for_closing_double_quote(void) {
	free_errors();

	Token *t = tokenize(U"\"");

	ASSERT_FALSEY(t);
	ASSERT_FALSEY(compare_errors(
		"(null): Compilation error: line 1 column 1: expected closing quote\n"
	));

	PASS
}

void tokenizer_test_self(bool *pass) {
	RUN_ALL(
		test_tokenize_single_token,
		test_tokenize_no_tokens,
		test_whitespace_between_tokens,
		test_whitespace_at_eol_ignored,
		test_whitespace_inside_double_quotes_respected,
		test_whitespace_inside_single_quotes_respected,
		test_brackets_always_make_their_own_token,
		test_newlines_always_make_their_own_token,
		test_token_len,
		test_token_cmp,
		test_token_cmp_match_exact_strings_only,
		test_token_to_string,
		test_tokenize_comment,
		test_tokenize_trailing_comment,
		test_make_token,
		test_tokenize_with_lines_and_columns,
		test_nested_block_comment_fails,
		test_warn_bom,
		test_invalid_comment_start,
		test_check_for_missing_block_comment,
		test_check_for_closing_single_quote,
		test_check_for_closing_double_quote
	)

	free_errors();
}
