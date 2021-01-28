#pragma once

#include "skull/parse/ast_node.h"

void str_to_llvm_ir(char *const);
_Bool node_to_llvm_ir(AstNode *);
