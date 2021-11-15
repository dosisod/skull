#pragma once

#include "skull/parse/ast_node.h"

typedef char * CStmt;
typedef char * CBlock;

CStmt return_node_to_string(const AstNode *);
CStmt noop_to_string(const AstNode *);
CBlock if_to_string(const AstNode *);
