#pragma once

#include "skull/eval/scope.h"
#include "skull/eval/variable.h"
#include "skull/parse/ast/node.h"

char32_t *eval_assign(Variable *const, const AstNode *const, const Scope *);