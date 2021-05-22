#pragma once

#include "skull/codegen/llvm_fwd_decl.h"
#include "skull/compiler/types/types.h"
#include "skull/parse/ast_node.h"

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

Expr gen_expr_oper(Type, const AstNodeExpr *const, _Bool *);
