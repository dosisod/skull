#pragma once

#include <llvm-c/Core.h>

#include "skull/eval/scope.h"
#include "skull/eval/variable.h"
#include "skull/parse/ast/node.h"

void llvm_make_assign_(Variable *const, const AstNode *const);

void node_make_var(const AstNode *const, Scope *const);
