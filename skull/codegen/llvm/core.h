#pragma once

#include "skull/codegen/llvm/expr.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/parse/ast_node.h"

typedef struct AstNode AstNode;

Expr gen_tree(const AstNode *);
void gen_module(const AstNode *, SkullStateLLVM *);
