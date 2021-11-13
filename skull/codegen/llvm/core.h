#pragma once

#include "skull/codegen/llvm/expr.h"
#include "skull/parse/ast_node.h"

typedef struct AstNode AstNode;

Expr gen_tree(AstNode *);
void gen_module(AstNode *);
