#pragma once

typedef struct AstNodeVarAssign AstNodeVarAssign;
typedef struct AstNodeVarDef AstNodeVarDef;

void gen_stmt_var_def(AstNodeVarDef *);
void gen_stmt_var_assign(AstNodeVarAssign *);
