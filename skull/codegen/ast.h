#pragma once

#include "skull/codegen/var.h"
#include "skull/parse/ast_node.h"

void codegen_str(char *const);
Expr gen_node(AstNode *);
