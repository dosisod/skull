#pragma once

#include <llvm-c/Core.h>

#include "skull/codegen/var.h"
#include "skull/parse/ast_node.h"

typedef Expr (Operation)(const Type *const, LLVMValueRef, LLVMValueRef);

Expr gen_expr_oper(const Type *const, const AstNodeExpr *const, Operation *);

Operation gen_expr_add;
Operation gen_expr_sub;
Operation gen_expr_mult;
Operation gen_expr_div;
Operation gen_expr_mod;
Expr gen_expr_not(const Type *, const AstNodeExpr *);
Expr gen_expr_unary_neg(const Type *, const AstNodeExpr *);
Operation gen_expr_is;
Operation gen_expr_is_not;
Operation gen_expr_less_than;
Operation gen_expr_gtr_than;
Operation gen_expr_less_than_eq;
Operation gen_expr_gtr_than_eq;
Operation gen_expr_lshift;
Operation gen_expr_rshift;
Operation gen_expr_and;
Operation gen_expr_or;
Operation gen_expr_xor;
Operation gen_expr_pow;
