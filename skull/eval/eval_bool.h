#pragma once

#include "skull/parse/tokenize.h"

bool eval_bool_true(const token_t *, const char32_t **);
bool eval_equality_comparison(const token_t *, const char32_t **);
bool eval_bool(const token_t *, const char32_t **);