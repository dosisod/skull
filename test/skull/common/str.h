#pragma once

#include <uchar.h>

#include "test/testing.h"

_Bool c32sunescape_wrapper(const char32_t *, const char32_t, const char32_t *);

TEST_DECL(strrstr)
TEST_DECL(c32sncpy)
TEST_DECL(c32sncpy_fill_nulls)
TEST_DECL(c32slcpy)
TEST_DECL(c32sdup)
TEST_DECL(c32scat)
TEST_DECL(c32slen)
TEST_DECL(c32stombs)
TEST_DECL(c32stombs_with_null)
TEST_DECL(mbstoc32s)
TEST_DECL(c32scmp)
TEST_DECL(c32sncmp)
TEST_DECL(c32schr)
TEST_DECL(c32isdigit)
TEST_DECL(c32isalnum)
TEST_DECL(c32isxdigit)
TEST_DECL(c32sunescape)

void str_test_self(_Bool *);
