#pragma once

#include "skull/eval/variable.h"

char32_t *fmt_int_type(const Variable *);

Variable *add_int_type(const Variable *, const Variable *);
Variable *sub_int_type(const Variable *, const Variable *);
Variable *div_int_type(const Variable *, const Variable *);
Variable *mult_int_type(const Variable *, const Variable *);
