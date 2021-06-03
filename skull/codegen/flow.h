#pragma once

#include "skull/codegen/expr.h"
#include "skull/parse/ast_node.h"

Expr gen_stmt_return(AstNode *, _Bool *);
Expr gen_stmt_unreachable(void);
_Bool gen_control_while(AstNode *);
_Bool gen_control_if(AstNode **);
