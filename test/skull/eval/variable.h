#pragma once

#include "test/testing.h"

#define TEST_FMT_VAR(type, real_type, real_data, expected) \
	Variable *var = make_variable(type, U"x", false); \
	real_type data = real_data; \
	variable_write(var, &data); \
	char32_t *str = fmt_var(var); \
	ASSERT_TRUTHY(str); \
	ASSERT_TRUTHY(c32scmp(expected, str)); \
	free(str); \
	free_variable(var)

void variable_test_self(_Bool *pass);
