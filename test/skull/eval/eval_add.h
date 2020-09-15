#pragma once

#include "test/testing.h"

#define eval_add(x, y) eval_oper((x)->type->add, x, y)

TEST_DECL(adding_2_ints)
TEST_DECL(adding_2_floats)
TEST_DECL(adding_2_strs)
TEST_DECL(adding_vars_with_different_types_fail)

void eval_add_test_self(_Bool *);
