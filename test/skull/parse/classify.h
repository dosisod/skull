#include "test/testing.h"

#define TEST_CLASSIFY_TOKEN(str, expected) \
	const char32_t *code=str; \
	Token *token=tokenize(code); \
	classify_token(token); \
	const bool pass=(token->token_type==(expected)); \
	free(token); \
	return pass

#define TEST_CLASSIFY_TOKEN_WITH_LEN(str, tok_type, start_at, end_at) \
	const char32_t *code=(str); \
	Token *t=tokenize(code); \
	classify_tokens(t); \
	const bool pass=( \
		t->token_type==(tok_type) && \
		t->begin==(code + (start_at)) && \
		t->end==(code + (end_at)) \
	); \
	free(t); \
	return pass

TEST_DECL(is_type_str)
TEST_DECL(is_keyword_str)
TEST_DECL(is_constant_integer)
TEST_DECL(is_constant_float)
TEST_DECL(is_constant_bool)
TEST_DECL(is_constant_rune)
TEST_DECL(is_constant_str)
TEST_DECL(token_newline)
TEST_DECL(bracket_token_open)
TEST_DECL(bracket_token_close)
TEST_DECL(token_mut_kw)
TEST_DECL(token_return_kw)
TEST_DECL(token_if_kw)
TEST_DECL(token_import_kw)
TEST_DECL(token_from_kw)
TEST_DECL(token_equal_oper)
TEST_DECL(token_auto_equal_oper)
TEST_DECL(token_plus_oper)
TEST_DECL(token_minus_oper)
TEST_DECL(token_mult_oper)
TEST_DECL(token_div_oper)
TEST_DECL(token_type)
TEST_DECL(token_unknown)
TEST_DECL(token_integer_constant)
TEST_DECL(token_float_constant)
TEST_DECL(token_bool_constant)
TEST_DECL(token_rune_constant)
TEST_DECL(token_str_constant)
TEST_DECL(token_Typeype)
TEST_DECL(token_comment)
TEST_DECL(token_comment_empty)
TEST_DECL(is_valid_identifier)
TEST_DECL(is_valid_identifier_token)
TEST_DECL(new_identifier_clip_trailing_colon)
TEST_DECL(identifier_cannot_be_type)
TEST_DECL(identifier_cannot_be_keyword)
TEST_DECL(identifier_cannot_be_func_name)
TEST_DECL(classify_tokens)

void classifier_test_self(_Bool *pass);
