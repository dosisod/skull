#pragma once

#include "skull/parse/ast_node.h"

typedef struct AstNode AstNode;
typedef char * CTree;

void gen_module_c(const AstNode *);
CTree gen_tree_c(const AstNode *);
