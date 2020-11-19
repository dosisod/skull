#pragma once

#include <llvm-c/Core.h>

#include "skull/parse/ast/node.h"

typedef LLVMValueRef (MathOper)(const Type *, LLVMValueRef, LLVMValueRef);

LLVMValueRef llvm_make_math_oper(const Type *, const AstNode *, MathOper *);

MathOper llvm_make_add;
MathOper llvm_make_sub;
MathOper llvm_make_mult;
MathOper llvm_make_div;
