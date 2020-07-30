#pragma once

#include "skull/eval/variable.h"

char32_t *fmt_int_type(const variable_t *);

variable_t *add_int_type(const variable_t *, const variable_t *);
variable_t *sub_int_type(const variable_t *, const variable_t *);
variable_t *div_int_type(const variable_t *, const variable_t *);
variable_t *mult_int_type(const variable_t *, const variable_t *);
