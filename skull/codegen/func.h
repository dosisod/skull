#pragma once

#include "skull/codegen/expr.h"
#include "skull/codegen/llvm/fwd_decl.h"

typedef struct FunctionDeclaration FunctionDeclaration;
typedef struct AstNodeFunctionParam AstNodeFunctionParam;
typedef struct AstNodeFunctionCall AstNodeFunctionCall;
typedef struct AstNode AstNode;
typedef struct AstNodeExpr AstNodeExpr;

_Bool gen_stmt_func_decl(const AstNode *const);

Expr gen_expr_func_call(const AstNodeFunctionCall *const, Type, _Bool *);
