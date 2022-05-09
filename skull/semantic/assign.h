#pragma once

#include "skull/parse/ast_node.h"
#include "skull/semantic/shared.h"

_Bool validate_stmt_var_def(SemanticState *, const AstNode *);
_Bool validate_stmt_var_assign(SemanticState *, const AstNode *);
