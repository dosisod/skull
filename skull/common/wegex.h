#pragma once

#include <stdbool.h>
#include <uchar.h>

bool wegex_cmp(const char *, const char *, char32_t);
const char *find_next_wegex(const char *);

bool wegex_match(const char *, const char32_t *);