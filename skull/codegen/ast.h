#pragma once

#include <stdbool.h>

#include "skull/codegen/var.h"
#include "skull/parse/ast_node.h"

bool codegen_str(char *const);
bool assert_sane_child(AstNode *);
Expr gen_node(AstNode *, bool *);
