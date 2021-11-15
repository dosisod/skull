#pragma once

#include "skull/parse/ast_node.h"

typedef char * CExpr;

CExpr expr_node_to_string(const AstNodeExpr *);
