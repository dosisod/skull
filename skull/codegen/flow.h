#pragma once

#include "skull/codegen/expr.h"

typedef struct AstNode AstNode;

Expr gen_stmt_return(AstNode *, _Bool *);
Expr gen_stmt_unreachable(void);
void gen_stmt_implicit_main_return(void);
_Bool gen_control_while(AstNode *);
_Bool gen_control_if(AstNode **);
