#pragma once

#include "skull/codegen/expr.h"
#include "skull/codegen/llvm/fwd_decl.h"
#include "skull/compiler/types.h"

typedef struct FunctionDeclaration FunctionDeclaration;
typedef struct AstNodeFunctionParam AstNodeFunctionParam;
typedef struct AstNode AstNode;
typedef struct AstNodeExpr AstNodeExpr;

_Bool gen_stmt_func_decl(const AstNode *const);

Expr gen_expr_function_call(const AstNodeExpr *const, Type, _Bool *);
