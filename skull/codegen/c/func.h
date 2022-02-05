#pragma once

#include "skull/codegen/c/shared.h"
#include "skull/semantic/func.h"

typedef char * CStmt;

void gen_function_def_c(const AstNode *, SkullStateC *);
CStmt gen_function_prototype_c(const AstNode *, SkullStateC *);
CStmt gen_stmt_func_call_c(const AstNode *, SkullStateC *);
