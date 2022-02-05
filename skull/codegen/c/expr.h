#pragma once

#include "skull/codegen/c/shared.h"
#include "skull/parse/ast_node.h"

typedef char * CExpr;

CExpr gen_expr_c(const AstNodeExpr *, SkullStateC *);
