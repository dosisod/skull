#pragma once

#include <llvm-c/Core.h>

#include "skull/parse/ast_node.h"

typedef LLVMValueRef (Operation)(const Type *, LLVMValueRef, LLVMValueRef);

LLVMValueRef llvm_make_oper(const Type *, const AstNode *const, Operation *);
LLVMValueRef llvm_token_to_val(const Type *, const Token *const);

Operation llvm_make_add;
Operation llvm_make_sub;
Operation llvm_make_mult;
Operation llvm_make_div;
Operation llvm_make_is;
