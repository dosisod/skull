#pragma once

typedef struct AstNodeVarAssign AstNodeVarAssign;
typedef struct AstNodeVarDef AstNodeVarDef;

#include "skull/codegen/llvm/shared.h"

void gen_stmt_var_def(const AstNodeVarDef *, SkullStateLLVM *);
void gen_stmt_var_assign(const AstNodeVarAssign *, SkullStateLLVM *);
