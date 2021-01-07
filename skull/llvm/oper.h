#pragma once

#include <llvm-c/Core.h>

#include "skull/llvm/var.h"
#include "skull/parse/ast_node.h"

typedef Expr (Operation)(const Type *const, LLVMValueRef, LLVMValueRef);

Expr llvm_make_oper(const Type *const, const AstNode *const, Operation *);
LLVMValueRef token_to_llvm_value(const Type *const, const Token *const);

Operation llvm_make_add;
Operation llvm_make_sub;
Operation llvm_make_mult;
Operation llvm_make_div;
Operation llvm_make_is;
