#pragma once

#include <llvm-c/Core.h>

#include "skull/eval/scope.h"
#include "skull/eval/variable.h"
#include "skull/parse/ast_node.h"

typedef struct {
	LLVMValueRef llvm_value;
	const Type *type;
} Expr;

Expr llvm_token_get_value(const Token *const, Variable **);

void node_make_var(const AstNode *const);

Expr llvm_parse_token_typed(const Type *const, const Token *const);
Expr llvm_parse_token(const Token *const);
