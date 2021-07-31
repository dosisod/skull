#pragma once

#include "skull/codegen/expr.h"
#include "skull/parse/ast_node.h"

typedef struct AstNode AstNode;

Expr gen_node(AstNode *, _Bool *);
_Bool gen_tree(AstNode *);
