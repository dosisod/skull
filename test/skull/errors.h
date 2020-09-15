#pragma once

#include <uchar.h>

#include "skull/errors.h"
#include "test/testing.h"

_Bool fmt_error_wrapper(const char32_t *, ErrorMsg [], const char32_t *);
_Bool fmt_error_stringify_wrapper(ErrorMsg *, const char32_t *);

TEST_DECL(is_error_msg)
TEST_DECL(fmt_error)
TEST_DECL(fmt_error_stringify)

void error_test_self(_Bool *);
