#pragma once

#include "skull/parse/ast/node.h"

void llvm_make_return(AstNode *);
void llvm_make_while(AstNode *);
void llvm_make_if(AstNode **);
