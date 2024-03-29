#pragma once

#include "skull/codegen/llvm/core.h"
#include "skull/codegen/llvm/expr.h"
#include "skull/codegen/llvm/fwd_decl.h"

typedef struct FunctionDeclaration FunctionDeclaration;
typedef struct AstNodeFunctionParam AstNodeFunctionParam;
typedef struct AstNodeFunctionCall AstNodeFunctionCall;
typedef struct AstNode AstNode;
typedef struct AstNodeExpr AstNodeExpr;

void gen_stmt_func_decl(const AstNode *const, SkullStateLLVM *);
Expr gen_expr_func_call(
	const AstNodeFunctionCall *const,
	const SkullStateLLVM *
);
