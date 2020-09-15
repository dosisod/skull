#pragma once

#include "test/testing.h"

TEST_DECL(make_scope)
TEST_DECL(scope_find_name)
TEST_DECL(add_vars_to_scope)
TEST_DECL(cannot_add_same_varname_to_scope)
TEST_DECL(free_scope)
TEST_DECL(scope_find_name_when_null)

void scope_test_self(_Bool *pass);