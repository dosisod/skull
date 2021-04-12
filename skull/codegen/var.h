#pragma once

#include <llvm-c/Core.h>

#include "skull/compiler/variable.h"
#include "skull/parse/ast_node.h"

/*
Expr stores all the data needed to do type comparisons with LLVMValueRef's.

`llvm_value` is the LLVM component.

`type` is the Skull usable which `llvm_value` evaluates to.
*/
typedef struct {
	LLVMValueRef llvm_value;
	const Type *type;
} Expr;

Variable *node_to_var(const AstNode *const);
