#pragma once

#include "skull/parse/ast_node.h"

void gen_stmt_return(AstNode **);
void gen_control_while(AstNode **);
void gen_control_if(AstNode **);
