#pragma once

#include "skull/compiler/variable.h"
#include "skull/parse/ast_node.h"

_Bool gen_stmt_var_def(AstNode *);
_Bool gen_stmt_var_assign(AstNode *);
