#pragma once

#include <llvm-c/Core.h>

#include "skull/eval/variable.h"
#include "skull/parse/ast/node.h"

void str_to_llvm_ir(char *const, LLVMValueRef, LLVMModuleRef);
void node_to_llvm_ir(AstNode *);

void llvm_make_return(AstNode *);
void llvm_make_var_def(AstNode **);
void llvm_make_if(AstNode *);
void llvm_make_add(Variable *, const AstNode *);
void llvm_make_function(AstNode *);
void llvm_make_assign(AstNode **);
