#pragma once

#include "skull/codegen/var.h"
#include "skull/parse/ast_node.h"

void codegen_str(char *const);
void assert_sane_child(AstNode *);
Expr gen_node(AstNode *);
