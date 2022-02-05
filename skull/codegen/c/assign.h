#pragma once

#include "skull/codegen/c/shared.h"
#include "skull/parse/ast_node.h"

typedef char * CStmt;

CStmt gen_stmt_var_assign_c(const AstNode *, SkullStateC *);
CStmt gen_stmt_var_def_c(const AstNode *, SkullStateC *);
