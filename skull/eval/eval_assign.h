#pragma once

#include "skull/eval/context.h"
#include "skull/eval/variable.h"
#include "skull/parse/ast/node.h"

const char32_t *eval_assign(Variable *, AstNode *, const Context *);