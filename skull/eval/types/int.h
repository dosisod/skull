#pragma once

#include "skull/eval/types/defs.h"
#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char *fmt_int_type(const Variable *);

SkullInt eval_integer(const Token *, char32_t **);
