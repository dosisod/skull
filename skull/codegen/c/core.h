#pragma once

#include "skull/parse/ast_node.h"

typedef struct AstNode AstNode;

void gen_module_c(const AstNode *);
char *gen_tree_c(const AstNode *);
