#include <stdbool.h>

#include "skull/codegen/aliases.h"
#include "skull/codegen/scope.h"
#include "skull/codegen/types.h"
#include "skull/common/errors.h"
#include "skull/common/panic.h"
#include "skull/compiler/types/float.h"
#include "skull/compiler/types/int.h"

#include "skull/codegen/oper.h"

typedef LLVMValueRef (LLVMBuildX)(
	LLVMBuilderRef,
	LLVMValueRef,
	LLVMValueRef,
	const char *
);

/*
Returns the result of a mathematical operation on `lhs` and `rhs`.

Depending on whether `type` is an int or float, combine `lhs` and `rhs`
using `int_func` or `float_func`.
*/
Expr gen_expr_math_oper(
	const Type *const type,
	LLVMValueRef lhs,
	LLVMValueRef rhs,
	LLVMBuildX int_func,
	LLVMBuildX float_func
) {
	if (type == &TYPE_INT)
		return (Expr){
			.llvm_value = int_func(SKULL_STATE.builder, lhs, rhs, ""),
			.type = type
		};

	if (type == &TYPE_FLOAT)
		return (Expr){
			.llvm_value = float_func(SKULL_STATE.builder, lhs, rhs, ""),
			.type = type
		};

	return (Expr){0};
}

/*
Return expression for addition of `lhs` and `rhs`.
*/
Expr gen_expr_add(
	const Type *const type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	return gen_expr_math_oper(type, lhs, rhs, LLVMBuildNSWAdd, LLVMBuildFAdd);
}

/*
Return expression for subtraction of `lhs` and `rhs`.
*/
Expr gen_expr_sub(
	const Type *const type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	return gen_expr_math_oper(type, lhs, rhs, LLVMBuildNSWSub, LLVMBuildFSub);
}

/*
Return expression for multiplication of `lhs` and `rhs`.
*/
Expr gen_expr_mult(
	const Type *const type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	return gen_expr_math_oper(type, lhs, rhs, LLVMBuildNSWMul, LLVMBuildFMul);
}

/*
Return expression for division of `lhs` and `rhs`.
*/
Expr gen_expr_div(
	const Type *const type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	if (type == &TYPE_INT) {
		if (LLVMConstIntGetSExtValue(rhs) == 0) {
			PANIC(ERR_DIV_BY_ZERO, {0});
		}
	}

	return gen_expr_math_oper(
		type,
		lhs,
		rhs,
		LLVMBuildExactSDiv,
		LLVMBuildFDiv
	);
}

/*
Return expression for modulus of `lhs` and `rhs`.
*/
Expr gen_expr_mod(
	const Type *const type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	if (type == &TYPE_INT) {
		if (LLVMConstIntGetSExtValue(rhs) == 0) {
			PANIC(ERR_DIV_BY_ZERO, {0});
		}
	}

	return gen_expr_math_oper(
		type,
		lhs,
		rhs,
		LLVMBuildSRem,
		LLVMBuildFRem
	);
}

/*
Return expression for left shift of `lhs` and `rhs`.
*/
Expr gen_expr_lshift(
	const Type *const type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	if (type == &TYPE_INT)
		return (Expr){
			.llvm_value = LLVMBuildShl(SKULL_STATE.builder, lhs, rhs, ""),
			.type = &TYPE_INT
		};

	return (Expr){0};
}

/*
Return expression for logical right shift of `lhs` and `rhs`.
*/
Expr gen_expr_rshift(
	const Type *const type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	if (type == &TYPE_INT)
		return (Expr){
			.llvm_value = LLVMBuildLShr(SKULL_STATE.builder, lhs, rhs, ""),
			.type = &TYPE_INT
		};

	return (Expr){0};
}

Expr create_and_call_builtin_oper(
	const Type *,
	LLVMTypeRef,
	const char *,
	LLVMValueRef,
	LLVMValueRef
);

/*
Return expression for taking `lhs` to the power of `rhs`.
*/
Expr gen_expr_pow(
	const Type *const type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	const char *func_name = NULL;

	if (type == &TYPE_INT)
		func_name = "_int_pow";

	else if (type == &TYPE_FLOAT)
		func_name = "_float_pow";

	else {
		PANIC(ERR_POW_BAD_TYPE, { .type = type });
	}

	return create_and_call_builtin_oper(
		type,
		gen_llvm_type(type),
		func_name,
		lhs,
		rhs
	);
}

/*
Return expression for result of not operator for `rhs`.
*/
Expr gen_expr_not(const Type *type, LLVMValueRef lhs, LLVMValueRef rhs) {
	(void)type;
	(void)lhs;

	return (Expr){
		.llvm_value = LLVMBuildNot(SKULL_STATE.builder, rhs, ""),
		.type = &TYPE_BOOL
	};
}

/*
Return expression for result of unary negation operator for `rhs`.
*/
Expr gen_expr_unary_neg(const Type *type, LLVMValueRef lhs, LLVMValueRef rhs) {
	(void)lhs;

	return gen_expr_math_oper(
		type,
		LLVM_INT(0),
		rhs,
		LLVMBuildNSWSub,
		LLVMBuildFSub
	);
}


Expr gen_expr_is_str(LLVMValueRef, LLVMValueRef);

/*
Return expression for result of is operator for `lhs` and `rhs`.
*/
Expr gen_expr_is(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (type == &TYPE_INT || type == &TYPE_RUNE)
		return (Expr){
			.llvm_value = LLVMBuildICmp(
				SKULL_STATE.builder,
				LLVMIntEQ,
				lhs,
				rhs,
				""
			),
			.type = &TYPE_BOOL
		};

	if (type == &TYPE_FLOAT)
		return (Expr){
			.llvm_value = LLVMBuildFCmp(
				SKULL_STATE.builder,
				LLVMRealOEQ,
				lhs,
				rhs,
				""
			),
			.type = &TYPE_BOOL
		};

	if (type == &TYPE_STR)
		return gen_expr_is_str(lhs, rhs);

	return (Expr){0};
}

/*
Return expression for string-is operator against `lhs` and `rhs`.
*/
Expr gen_expr_is_str(LLVMValueRef lhs, LLVMValueRef rhs) {
	return create_and_call_builtin_oper(
		&TYPE_BOOL,
		gen_llvm_type(&TYPE_STR),
		"_strcmp",
		lhs,
		rhs
	);
}

/*
Create a function called `name` (if it does not exist) which returns type
`rtype`, and has operands of type `type`. Afterwards, call the new function
with the `lhs` and `rhs` operands.
*/
Expr create_and_call_builtin_oper(
	const Type *rtype,
	LLVMTypeRef type,
	const char *name,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	LLVMValueRef func = LLVMGetNamedFunction(SKULL_STATE.module, name);

	LLVMTypeRef func_type = LLVMFunctionType(
		gen_llvm_type(rtype),
		(LLVMTypeRef[]){ type, type },
		2,
		false
	);

	if (!func)
		func = LLVMAddFunction(
			SKULL_STATE.module,
			name,
			func_type
		);

	return (Expr){
		.llvm_value = LLVMBuildCall2(
			SKULL_STATE.builder,
			func_type,
			func,
			(LLVMValueRef[]){ lhs, rhs },
			2,
			""
		),
		.type = rtype
	};
}

/*
Return expression for result of is not operator for `lhs` and `rhs`.
*/
Expr gen_expr_is_not(
	const Type *const type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	Expr expr = gen_expr_is(type, lhs, rhs);

	expr.llvm_value = LLVMBuildNot(
		SKULL_STATE.builder,
		expr.llvm_value,
		""
	);

	return expr;
}

/*
Return result of relational comparison on `lhs` and `rhs`.

Depending on whether `type` is an int or float, compare `lhs` and `rhs` using
`int_func` or `float_func`.
*/
Expr gen_expr_relational_oper(
	const Type *type,
	LLVMValueRef lhs,
	LLVMValueRef rhs,
	LLVMIntPredicate int_pred,
	LLVMRealPredicate float_pred
) {
	if (type == &TYPE_INT)
		return (Expr){
			.llvm_value = LLVMBuildICmp(
				SKULL_STATE.builder,
				int_pred,
				lhs,
				rhs,
				""
			),
			.type = &TYPE_BOOL
		};

	if (type == &TYPE_FLOAT)
		return (Expr){
			.llvm_value = LLVMBuildFCmp(
				SKULL_STATE.builder,
				float_pred,
				lhs,
				rhs,
				""
			),
			.type = &TYPE_BOOL
		};

	return (Expr){0};
}

/*
Return expression for result of less than operator for `lhs` and `rhs`.
*/
Expr gen_expr_less_than(
	const Type *const type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	return gen_expr_relational_oper(type, lhs, rhs, LLVMIntSLT, LLVMRealOLT);
}

/*
Return expression for result of greater than operator for `lhs` and `rhs`.
*/
Expr gen_expr_gtr_than(
	const Type *const type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	return gen_expr_relational_oper(type, lhs, rhs, LLVMIntSGT, LLVMRealOGT);
}

/*
Return expression for result of less than or equal to operator for `lhs` and
`rhs`.
*/
Expr gen_expr_less_than_eq(
	const Type *const type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	return gen_expr_relational_oper(type, lhs, rhs, LLVMIntSLE, LLVMRealOLE);
}

/*
Return expression for result of greater than or equal to operator for `lhs`
and `rhs`.
*/
Expr gen_expr_gtr_than_eq(
	const Type *const type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	return gen_expr_relational_oper(type, lhs, rhs, LLVMIntSGE, LLVMRealOGE);
}

/*
Return result of logical operation `func` on `lhs` and `rhs`.
*/
Expr gen_expr_logical_oper(
	const Type *const type,
	LLVMValueRef lhs,
	LLVMValueRef rhs,
	LLVMBuildX func
) {
	return (Expr){
		.llvm_value = func(
			SKULL_STATE.builder,
			lhs,
			rhs,
			""
		),
		.type = type
	};
}

/*
Return result of logical "and" operation of `lhs` and `rhs`.
*/
Expr gen_expr_and(
	const Type *const type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	return gen_expr_logical_oper(type, lhs, rhs, LLVMBuildAnd);
}

/*
Return result of logical "or" operation of `lhs` and `rhs`.
*/
Expr gen_expr_or(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs) {
	return gen_expr_logical_oper(type, lhs, rhs, LLVMBuildOr);
}

/*
Return result of logical "xor" operation of `lhs` and `rhs`.
*/
Expr gen_expr_xor(
	const Type *const type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	return gen_expr_logical_oper(type, lhs, rhs, LLVMBuildXor);
}

/*
Return expression for identifier `token` with type `type`.

Optionally pass `var` if result is expected to be assigned to a variable.

If `type` is not set, the expression type will not be checked.
*/
Expr gen_expr_identifier(
	const Type *const type,
	const Token *const token,
	const Variable *const var
) {
	Variable *var_found = NULL;
	const Expr expr = token_to_expr(token, &var_found);

	if (type && var_found && var_found->type != type) {
		PANIC(ERR_EXPECTED_SAME_TYPE,
			{ .tok = token, .type = type },
			{ .type = var_found->type }
		);
	}
	if (var == var_found) {
		PANIC(ERR_REDUNDANT_REASSIGN, {
			.tok = token,
			.var = var
		});
	}

	return expr;
}

/*
Return expression for operation `oper` for `expr`.
*/
Expr gen_expr_oper(
	const Type *const type,
	const AstNodeExpr *const expr,
	const Variable *const var
) {
	Operation *func = NULL;

	switch (expr->oper) {
		case EXPR_ADD: func = &gen_expr_add; break;
		case EXPR_SUB: func = &gen_expr_sub; break;
		case EXPR_UNARY_NEG: func = &gen_expr_unary_neg; break;
		case EXPR_MULT: func = &gen_expr_mult; break;
		case EXPR_DIV: func = &gen_expr_div; break;
		case EXPR_MOD: func = &gen_expr_mod; break;
		case EXPR_NOT: func = &gen_expr_not; break;
		case EXPR_LSHIFT: func = &gen_expr_lshift; break;
		case EXPR_POW: func = &gen_expr_pow; break;
		case EXPR_RSHIFT: func = &gen_expr_rshift; break;
		case EXPR_IS: func = gen_expr_is; break;
		case EXPR_ISNT: func = gen_expr_is_not; break;
		case EXPR_LESS_THAN: func = gen_expr_less_than; break;
		case EXPR_GTR_THAN: func = gen_expr_gtr_than; break;
		case EXPR_LESS_THAN_EQ: func = gen_expr_less_than_eq; break;
		case EXPR_GTR_THAN_EQ: func = gen_expr_gtr_than_eq; break;
		case EXPR_AND: func = gen_expr_and; break;
		case EXPR_OR: func = gen_expr_or; break;
		case EXPR_XOR: func = gen_expr_xor; break;
		case EXPR_IDENTIFIER:
			return gen_expr_identifier(type, expr->lhs.tok, var);
		case EXPR_CONST:
			return gen_expr_const(type, expr->lhs.tok);
		case EXPR_FUNC:
			return gen_expr_function_call(expr, type);
		default: return (Expr){0};
	}

	const Token *lhs_token = expr->lhs.expr ? expr->lhs.expr->lhs.tok : NULL;
	const Expr lhs = lhs_token ? token_to_expr(lhs_token, NULL) : (Expr){0};

	const Token *rhs_token = expr->rhs.tok;
	const Expr rhs = token_to_expr(rhs_token, NULL);

	if (lhs.llvm_value && lhs.type != rhs.type) {
		PANIC(ERR_EXPECTED_SAME_TYPE,
			{ .tok = rhs_token, .type = lhs.type },
			{ .type = rhs.type }
		);
	}

	const Expr result = func(
		rhs.type,
		lhs.llvm_value,
		rhs.llvm_value
	);

	if (!result.type && !result.llvm_value) return (Expr){0};

	if (type && result.type != type) {
		const Token *tok = lhs_token ? lhs_token : rhs_token;

		PANIC(ERR_EXPECTED_SAME_TYPE,
			{ .tok = tok, .type = type },
			{ .type = result.type }
		);
	}

	return result;
}

#include "skull/compiler/types/bool.h"
#include "skull/compiler/types/defs.h"
#include "skull/compiler/types/float.h"
#include "skull/compiler/types/int.h"
#include "skull/compiler/types/rune.h"
#include "skull/compiler/types/str.h"

Expr token_to_simple_expr(const Token *const);

/*
Convert `token` to an expression.

If `variable` is and `token` is a variable, store the found variable there.
*/
Expr token_to_expr(const Token *const token, Variable **variable) {
	if (token->type == TOKEN_IDENTIFIER) {
		Variable *const var_found = scope_find_var(token);

		if (variable) *variable = var_found;

		if (var_found->is_const &&
			!(var_found->is_global &&
			!var_found->is_const_lit)
		) {
			return (Expr){
				.llvm_value = var_found->llvm_value,
				.type = var_found->type
			};
		}

		return (Expr) {
			.llvm_value = LLVMBuildLoad2(
				SKULL_STATE.builder,
				gen_llvm_type(var_found->type),
				var_found->llvm_value,
				""
			),
			.type = var_found->type
		};
	}

	return token_to_simple_expr(token);
}

/*
Make an expression from `token`, checking for compatibility with `type`.
*/
Expr gen_expr_const(
	const Type *const type,
	const Token *const token
) {
	const Expr expr = token_to_simple_expr(token);

	if (!expr.type) {
		PANIC(ERR_TYPE_MISMATCH, {
			.tok = token,
			.type = type
		});
	}

	return expr;
}

/*
Make a simple expression (const literal) from `token`.
*/
Expr token_to_simple_expr(const Token *const token) {
	LLVMValueRef llvm_value = NULL;
	const Type *type = NULL;

	if (token->type == TOKEN_INT_CONST) {
		llvm_value = LLVM_INT(eval_integer(token));
		type = &TYPE_INT;
	}
	else if (token->type == TOKEN_FLOAT_CONST) {
		llvm_value = LLVM_FLOAT(eval_float(token));
		type = &TYPE_FLOAT;
	}
	else if (token->type == TOKEN_BOOL_CONST) {
		llvm_value = LLVM_BOOL(eval_bool(token));
		type = &TYPE_BOOL;
	}
	else if (token->type == TOKEN_RUNE_CONST) {
		llvm_value = LLVM_RUNE(eval_rune(token));
		type = &TYPE_RUNE;
	}
	else if (token->type == TOKEN_STR_CONST) {
		SkullStr str = eval_str(token);
		char *const mbs = c32stombs(str);

		llvm_value = LLVMBuildBitCast(
			SKULL_STATE.builder,
			LLVMBuildGlobalString(SKULL_STATE.builder, mbs, ""),
			gen_llvm_type(&TYPE_STR),
			""
		);

		free(mbs);
		free(str);

		type = &TYPE_STR;
	}

	return (Expr){
		.llvm_value = llvm_value,
		.type = type
	};
}
