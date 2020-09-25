#pragma once

#include "test/testing.h"

#define TEST_FMT_VAR(str_type, real_type, real_data, expected) \
	Variable *var = make_variable(str_type, U"x", false); \
	real_type data = real_data; \
	variable_write(var, &data); \
	char32_t *str = fmt_var(var); \
	ASSERT_TRUTHY(str); \
	ASSERT_TRUTHY(c32scmp(expected, str)); \
	free(str); \
	free_variable(var)

TEST_DECL(create_variable)
TEST_DECL(create_variable_with_invalid_type_fails)
TEST_DECL(variable_write)
TEST_DECL(variable_cannot_write_to_const)
TEST_DECL(variable_read)
TEST_DECL(make_variable_with_invalid_name_fails)
TEST_DECL(free_variable)
TEST_DECL(free_null_variable_is_ok)
TEST_DECL(fmt_var_int)
TEST_DECL(fmt_var_float_zero)
TEST_DECL(fmt_var_float_small)
TEST_DECL(fmt_var_float_trailing_zero)
TEST_DECL(fmt_var_float_infinity)
TEST_DECL(fmt_var_float_neg_infinity)
TEST_DECL(fmt_var_bool)
TEST_DECL(fmt_var_type)
TEST_DECL(fmt_var_rune)
TEST_DECL(fmt_var_wide_rune_preserved)
TEST_DECL(fmt_var_str)
TEST_DECL(fmt_var_str_with_escapes)
TEST_DECL(fmt_var_str_with_bad_escape)

void variable_test_self(_Bool *pass);
