#pragma once

#include <llvm-c/Core.h>

#include "skull/eval/variable.h"
#include "skull/parse/ast/node.h"

void str_to_llvm_ir(char *const, LLVMValueRef, LLVMModuleRef);
void node_to_llvm_ir(AstNode *);

void llvm_make_return(AstNode *);
void llvm_make_var_def(AstNode **);
void llvm_make_if(AstNode *);

LLVMValueRef llvm_make_add(Variable *, const Token *, const Token *);
LLVMValueRef llvm_make_sub(Variable *, const Token *, const Token *);
LLVMValueRef llvm_make_mult(Variable *, const Token *, const Token *);
LLVMValueRef llvm_make_div(Variable *, const Token *, const Token *);

void llvm_make_function(AstNode *);
void llvm_make_assign(AstNode **);
