#pragma once

#include "skull/eval/variable.h"
#include "skull/parse/tokenize.h"

char *fmt_bool_type(const Variable *const);

_Bool eval_bool(const Token *const);
