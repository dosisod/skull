#pragma once

#include "skull/codegen/var.h"
#include "skull/compiler/variable.h"
#include "skull/parse/ast_node.h"

void create_type_alias(AstNode **);

void gen_stmt_var_def(AstNode **);
void gen_stmt_var_assign(AstNode **);

Expr node_to_expr(
	Type,
	const AstNode *const,
	const Variable *const
);
