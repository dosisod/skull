#pragma once

#include <stdbool.h>

#include "skull/codegen/expr.h"
#include "skull/parse/ast_node.h"

Expr gen_stmt_return(AstNode **, bool *);
Expr gen_stmt_unreachable(void);
bool gen_control_while(AstNode **);
bool gen_control_if(AstNode **);
