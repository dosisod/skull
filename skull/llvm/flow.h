#pragma once

#include "skull/parse/ast_node.h"

void llvm_make_return(AstNode **);
void llvm_make_while(const AstNode *const);
void llvm_make_if(AstNode **);
