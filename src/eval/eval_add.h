#pragma once

#include "variable.h"

/*
Make a new variable by adding the `lhs` and `rhs` together.
*/
variable_t *eval_add(const variable_t *lhs, const variable_t *rhs);