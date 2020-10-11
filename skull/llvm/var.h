#pragma once

#include <llvm-c/Core.h>

#include "skull/eval/scope.h"
#include "skull/eval/variable.h"
#include "skull/parse/ast/node.h"

void var_to_llvm_ir(Variable *);

void node_make_var(const AstNode *const, Scope *const);
