#pragma once

#include "skull/codegen/expr.h"
#include "skull/parse/ast_node.h"

_Bool assert_sane_child(AstNode *);
Expr gen_node(AstNode *, _Bool *);
_Bool gen_tree(AstNode *);
