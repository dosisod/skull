#pragma once

#include <stdbool.h>

#include "skull/compiler/variable.h"
#include "skull/parse/ast_node.h"

bool create_type_alias(AstNode **);

bool gen_stmt_var_def(AstNode **);
bool gen_stmt_var_assign(AstNode **);
