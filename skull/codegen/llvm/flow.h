#pragma once

#include "skull/codegen/llvm/expr.h"

typedef struct AstNode AstNode;
typedef struct Location Location;

Expr gen_stmt_return(const AstNode *);
Expr gen_stmt_unreachable(void);
Expr gen_stmt_break(void);
Expr gen_stmt_continue(void);
void gen_stmt_implicit_main_return(const Location *);
void gen_stmt_noop(const Location *);
void gen_control_while(const AstNode *);
void gen_control_if(const AstNode **);
