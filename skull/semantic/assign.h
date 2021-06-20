#pragma once

#include "skull/parse/ast_node.h"

_Bool validate_stmt_var_def(const AstNode *);
_Bool validate_stmt_var_assign(const AstNode *);
