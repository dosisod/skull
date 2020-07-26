#pragma once

#include <stdbool.h>
#include <uchar.h>

bool wegex_cmp(const char32_t *, const char32_t *, char32_t);
const char32_t *find_next_wegex(const char32_t *);

bool wegex_match(const char32_t *, const char32_t *);