#pragma once

#include "test/testing.h"

TEST_DECL(is_func_name)
TEST_DECL(is_func_name_with_null)
TEST_DECL(func_clear)
TEST_DECL(func_clear_params_fail)
TEST_DECL(func_print)
TEST_DECL(func_print_extra_params_fail)
TEST_DECL(func_print_bad_var)
TEST_DECL(free_ast_node_with_null)

void function_test_self(_Bool *pass);
