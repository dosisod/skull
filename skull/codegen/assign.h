#pragma once

typedef struct AstNodeVarAssign AstNodeVarAssign;
typedef struct AstNodeVarDef AstNodeVarDef;

_Bool gen_stmt_var_def(AstNodeVarDef *);
_Bool gen_stmt_var_assign(AstNodeVarAssign *);
