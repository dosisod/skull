#pragma once

#include "skull/codegen/llvm/fwd_decl.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/semantic/types.h"

typedef struct AstNode AstNode;
typedef struct AstNodeExpr AstNodeExpr;

/*
Expr stores all the data needed to do type comparisons with LLVMValueRef's.

`value` is the LLVM component.

`type` is the Skull usable which `value` evaluates to.
*/
typedef struct {
	LLVMValueRef value;
	const Type *type;
} Expr;

Expr gen_expr(const AstNodeExpr *const, const SkullStateLLVM *);
