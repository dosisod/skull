#pragma once

#include "skull/eval/context.h"
#include "skull/parse/ast/node.h"
#include "skull/parse/tokenize.h"

#include "skull/eval/variable.h"

const char32_t *eval_assign(Variable *, AstNode *, const Context *);