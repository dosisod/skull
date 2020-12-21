#pragma once

#include <llvm-c/Core.h>

#include "skull/eval/variable.h"
#include "skull/parse/ast_node.h"

void llvm_make_var_def(AstNode **);
void llvm_make_var_assign(AstNode **);
LLVMValueRef llvm_get_value_for_var(Variable *const, const AstNode *const);
LLVMValueRef llvm_assign_identifier(Variable *const, const AstNode *const);
