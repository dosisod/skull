#pragma once

#include "test/testing.h"

#define eval_div(x, y) eval_oper((x)->type->divide, x, y)

TEST_DECL(dividing_2_ints)
TEST_DECL(divide_by_zero)
TEST_DECL(dividing_2_floats)
TEST_DECL(dividing_vars_with_different_types_fail)

void eval_div_test_self(_Bool *);
