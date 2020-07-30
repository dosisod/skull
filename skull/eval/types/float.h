#pragma once

#include "skull/eval/variable.h"

char32_t *fmt_float_type(const variable_t *);

variable_t *add_float_type(const variable_t *, const variable_t *);
variable_t *sub_float_type(const variable_t *, const variable_t *);
variable_t *div_float_type(const variable_t *, const variable_t *);
variable_t *mult_float_type(const variable_t *, const variable_t *);
