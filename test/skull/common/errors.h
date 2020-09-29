#pragma once

#include <uchar.h>

#include "skull/common/errors.h"

_Bool fmt_error_wrapper(const char32_t *, ErrorMsg [], const char32_t *);
_Bool fmt_error_stringify_wrapper(ErrorMsg *, const char32_t *);

void error_test_self(_Bool *);
