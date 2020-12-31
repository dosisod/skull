#pragma once

#include <llvm-c/Core.h>

#include "skull/llvm/var.h"
#include "skull/parse/ast_node.h"

typedef Expr (Operation)(const Type *, LLVMValueRef, LLVMValueRef);

LLVMValueRef llvm_make_oper(const Type *, const AstNode *const, Operation *);
LLVMValueRef token_to_llvm_value(const Type *, const Token *const);

Operation llvm_make_add;
Operation llvm_make_sub;
Operation llvm_make_mult;
Operation llvm_make_div;

LLVMValueRef llvm_make_is(const Type *, LLVMValueRef, LLVMValueRef);
