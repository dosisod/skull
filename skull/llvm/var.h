#pragma once

#include <llvm-c/Core.h>

#include "skull/eval/scope.h"
#include "skull/eval/variable.h"
#include "skull/parse/ast_node.h"

typedef struct {
	LLVMValueRef llvm_value;
	const Type *type;
} Expr;

Expr token_to_expr(const Token *const, Variable **);

void node_make_var(const AstNode *const);

Expr token_to_simple_expr_typed(const Type *const, const Token *const);
Expr token_to_simple_expr(const Token *const);
