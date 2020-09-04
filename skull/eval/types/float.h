#pragma once

#include "skull/eval/variable.h"

char32_t *fmt_float_type(const Variable *);

Variable *add_float_type(const Variable *, const Variable *);
Variable *sub_float_type(const Variable *, const Variable *);
Variable *div_float_type(const Variable *, const Variable *);
Variable *mult_float_type(const Variable *, const Variable *);
