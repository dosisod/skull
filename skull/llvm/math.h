#pragma once

#include <llvm-c/Core.h>

#include "skull/eval/variable.h"
#include "skull/parse/ast/node.h"

typedef LLVMValueRef (*MathOper)(Variable *, const Token *, const Token *);

void llvm_make_math_oper(Variable *, const AstNode *, MathOper, const char *);

LLVMValueRef llvm_make_add(Variable *, const Token *, const Token *);
LLVMValueRef llvm_make_sub(Variable *, const Token *, const Token *);
LLVMValueRef llvm_make_mult(Variable *, const Token *, const Token *);
LLVMValueRef llvm_make_div(Variable *, const Token *, const Token *);
