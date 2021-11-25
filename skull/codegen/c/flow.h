#pragma once

#include "skull/parse/ast_node.h"

typedef char * CStmt;
typedef char * CBlock;

CStmt gen_stmt_return_c(const AstNode *);
CStmt gen_stmt_noop_c(const AstNode *);
CStmt gen_stmt_unreachable_c(const AstNode *);
CBlock gen_control_if_c(const AstNode *);
CBlock gen_control_elif_c(const AstNode *);
CBlock gen_control_else_c(const AstNode *);
CBlock gen_control_while_c(const AstNode *);
