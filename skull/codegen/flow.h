#pragma once

#include "skull/codegen/var.h"
#include "skull/parse/ast_node.h"

Expr gen_stmt_return(AstNode **);
Expr gen_stmt_unreachable(void);
void gen_control_while(AstNode **);
void gen_control_if(AstNode **);
