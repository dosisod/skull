#pragma once

#include "test/testing.h"

#define eval_mult(x, y) eval_oper((x)->type->multiply, x, y)

TEST_DECL(multiplying_2_ints)
TEST_DECL(multiplying_2_floats)
TEST_DECL(multiplying_vars_with_different_types_fail)

void eval_mult_test_self(_Bool *pass);
