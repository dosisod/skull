#pragma once

#include <uchar.h>

#include "skull/common/errors.h"

_Bool fmt_error_wrapper(const char *, ErrorMsg [], const char *);
_Bool fmt_error_stringify_wrapper(ErrorMsg *, const char *);

void error_test_self(_Bool *);
