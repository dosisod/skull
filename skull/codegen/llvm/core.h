#pragma once

#include "skull/codegen/llvm/expr.h"
#include "skull/codegen/llvm/shared.h"
#include "skull/parse/ast_node.h"

typedef struct AstNode AstNode;

Expr gen_tree(SemanticState *, const AstNode *, SkullStateLLVM *);
void gen_module(SemanticState *, const AstNode *, SkullStateLLVM *);
