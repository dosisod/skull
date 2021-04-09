#pragma once

#include <llvm-c/Core.h>

#include "skull/codegen/var.h"
#include "skull/parse/ast_node.h"

typedef Expr (Operation)(const Type *const, LLVMValueRef, LLVMValueRef);

Expr gen_expr_oper(
	const Type *const,
	const AstNode *const,
	const Variable *const
);

Operation gen_expr_add;
Operation gen_expr_sub;
Operation gen_expr_mult;
Operation gen_expr_div;
Operation gen_expr_mod;
Operation gen_expr_not;
Operation gen_expr_unary_neg;
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

Expr token_to_expr(const Token *const, Variable **);
Expr gen_expr_const(const Type *const, const Token *const);
Expr gen_expr_identifier(
	const Type *const,
	const Token *const,
	const Variable *const
);
