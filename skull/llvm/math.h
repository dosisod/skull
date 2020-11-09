#pragma once

#include <llvm-c/Core.h>

#include "skull/eval/variable.h"
#include "skull/parse/ast/node.h"

typedef LLVMValueRef (MathOper)(Variable *, LLVMValueRef, LLVMValueRef);

LLVMValueRef llvm_make_math_oper(Variable *, const AstNode *, MathOper *);

MathOper llvm_make_add;
MathOper llvm_make_sub;
MathOper llvm_make_mult;
MathOper llvm_make_div;
