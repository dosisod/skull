#pragma once

#include "skull/eval/scope.h"
#include "skull/parse/ast/node.h"

const char32_t *repl_make_var(const AstNode *, Scope *, _Bool);