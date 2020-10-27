#pragma once

#include "skull/eval/variable.h"
#include "skull/parse/ast/node.h"

void llvm_make_var_def(AstNode **);
void llvm_make_assign(AstNode **);
void llvm_make_assign_(Variable *const, const AstNode *const);
void llvm_assign_identifier(Variable *const, const AstNode *const);
