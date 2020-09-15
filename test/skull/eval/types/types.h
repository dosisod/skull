#pragma once

#include "test/testing.h"

TEST_DECL(make_new_type)
TEST_DECL(make_new_type_rejects_non_unique_type)
TEST_DECL(find_type)
TEST_DECL(free_types)
TEST_DECL(append_default_types)

void types_test_self(_Bool *pass);
