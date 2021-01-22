#pragma once

#include "skull/eval/variable.h"
#include "skull/llvm/var.h"
#include "skull/parse/ast_node.h"

void llvm_make_type_alias(AstNode **);

void llvm_make_var_def(AstNode **);
void llvm_make_var_assign(AstNode **);

Expr node_to_expr(
	const Type *const,
	const AstNode *const,
	const Variable *const
);

Expr llvm_assign_identifier(
	const Type *const,
	const AstNode *const,
	const Variable *const
);
