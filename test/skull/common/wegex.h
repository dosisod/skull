#pragma once

#include "test/testing.h"

TEST_DECL(no_modifiers)
TEST_DECL(star_modifier)
TEST_DECL(plus_modifier)
TEST_DECL(ternary_modifier)
TEST_DECL(charset_syntax)
TEST_DECL(wegex_number_match)
TEST_DECL(wegex_hexadecimal_match)
TEST_DECL(wegex_ascii_alpha_match)
TEST_DECL(wegex_full_alpha_match)
TEST_DECL(wegex_optional_modifiers_at_eol)

void wegex_test_self(_Bool *pass);
