#pragma once

#include "skull/codegen/expr.h"

typedef struct AstNode AstNode;
typedef struct Location Location;

Expr gen_stmt_return(AstNode *);
Expr gen_stmt_unreachable(void);
void gen_stmt_implicit_main_return(const Location *);
void gen_stmt_noop(const Location *);
_Bool gen_control_while(AstNode *);
_Bool gen_control_if(AstNode **);
