#pragma once

#include "context.h"
#include "../parse/tokenize.h"
#include "variable.h"

const char32_t *eval_assign(variable_t *, token_t *, const context_t *);