#pragma once

#include <llvm-c/Core.h>

#include "skull/eval/scope.h"
#include "skull/eval/variable.h"
#include "skull/parse/ast/node.h"

void node_make_var(const AstNode *const, Scope *const);

LLVMValueRef llvm_parse_var(const Variable *const, const Token *const);
