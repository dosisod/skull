#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "skull/parse/token.h"

int64_t eval_integer(const Token *, bool *);
