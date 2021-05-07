#pragma once

#include <stdbool.h>

#include "skull/codegen/var.h"
#include "skull/parse/ast_node.h"

bool codegen_str(char *const);
void assert_sane_child(AstNode *);
Expr gen_node(AstNode *);
