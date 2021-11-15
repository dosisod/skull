#pragma once

#include "skull/parse/ast_node.h"

typedef char * CStmt;

CStmt var_assign_to_string(const AstNode *);
CStmt var_def_to_string(const AstNode *);
