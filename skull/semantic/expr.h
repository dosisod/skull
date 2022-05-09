#pragma once

#include "skull/parse/ast_node.h"
#include "skull/semantic/shared.h"

_Bool validate_expr(SemanticState *, AstNodeExpr *);
_Bool validate_expr_func(SemanticState *, const AstNode *);
