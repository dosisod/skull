#pragma once

#include "skull/eval/variable.h"
#include "skull/llvm/var.h"
#include "skull/parse/ast_node.h"

void create_type_alias(AstNode **);

void gen_stmt_var_def(AstNode **);
void gen_stmt_var_assign(AstNode **);

Expr node_to_expr(
	const Type *const,
	const AstNode *const,
	const Variable *const
);
