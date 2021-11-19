#pragma once

#include "skull/parse/ast_node.h"

typedef char * CExpr;

CExpr gen_expr_c(const AstNodeExpr *);
CExpr gen_expr_func_call_c(const AstNodeFunctionCall *);
