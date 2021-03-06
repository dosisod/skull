#pragma once

#include "skull/codegen/llvm/fwd_decl.h"
#include "skull/semantic/types.h"

typedef struct AstNode AstNode;

/*
Expr stores all the data needed to do type comparisons with LLVMValueRef's.

`value` is the LLVM component.

`type` is the Skull usable which `value` evaluates to.
*/
typedef struct {
	LLVMValueRef value;
	Type type;
} Expr;

Expr node_to_expr(
	Type,
	const AstNode *const,
	_Bool *
);
