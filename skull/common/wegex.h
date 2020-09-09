#pragma once

#include <uchar.h>

_Bool wegex_cmp(const char *, const char *, char32_t);
const char *find_next_wegex(const char *);

_Bool wegex_match(const char *, const char32_t *);