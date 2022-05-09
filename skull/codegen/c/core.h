#pragma once

#include "skull/parse/ast_node.h"
#include "skull/codegen/c/shared.h"

typedef struct AstNode AstNode;
typedef char * CTree;

void gen_module_c(SemanticState *, const AstNode *, SkullStateC *);
CTree gen_tree_c(const AstNode *, SkullStateC *);
