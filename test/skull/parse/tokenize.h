#pragma once

#include "test/testing.h"

TEST_DECL(is_whitespace)
TEST_DECL(is_quote)
TEST_DECL(tokenize_single_token)
TEST_DECL(tokenize_no_tokens)
TEST_DECL(whitespace_between_tokens)
TEST_DECL(whitespace_at_eol_ignored)
TEST_DECL(whitespace_inside_double_quotes_respected)
TEST_DECL(whitespace_inside_single_quotes_respected)
TEST_DECL(brackets_always_make_their_own_token)
TEST_DECL(newlines_always_make_their_own_token)
TEST_DECL(free_tokens)
TEST_DECL(token_len)
TEST_DECL(token_cmp)
TEST_DECL(token_cmp_match_exact_strings_only)
TEST_DECL(token_str)
TEST_DECL(tokenize_comment)
TEST_DECL(tokenize_trailing_comment)
TEST_DECL(make_token)

void tokenizer_test_self(_Bool *pass);
