#pragma once

#include "skull/codegen/llvm/expr.h"
#include "skull/codegen/llvm/shared.h"

typedef struct AstNode AstNode;
typedef struct Location Location;

Expr gen_stmt_return(const AstNode *, const SkullStateLLVM *);
Expr gen_stmt_unreachable(const SkullStateLLVM *);
Expr gen_stmt_break(const SkullStateLLVM *);
Expr gen_stmt_continue(const SkullStateLLVM *);
void gen_stmt_implicit_main_return(const Location *, const SkullStateLLVM *);
void gen_stmt_noop(const Location *, const SkullStateLLVM *);
void gen_control_while(const AstNode *, SkullStateLLVM *);
void gen_control_if(const AstNode **, SkullStateLLVM *);
