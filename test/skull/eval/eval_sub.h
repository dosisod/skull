#pragma once

#include "test/testing.h"

#define eval_sub(x, y) eval_oper((x)->type->subtract, x, y)

TEST_DECL(subtracting_2_ints)
TEST_DECL(subtracting_2_floats)
TEST_DECL(subtracting_vars_with_different_types_fail)

void eval_sub_test_self(_Bool *pass);
