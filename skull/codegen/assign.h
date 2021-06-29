#pragma once

#include "skull/compiler/variable.h"

typedef struct AstNode AstNode;

_Bool gen_stmt_var_def(AstNode *);
_Bool gen_stmt_var_assign(AstNode *);
