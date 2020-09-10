#pragma once

#include "skull/eval/context.h"
#include "skull/parse/ast/node.h"

const char32_t *repl_make_var(const AstNode *, Context *, _Bool);