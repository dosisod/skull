#include <stdbool.h>

#include "skull/common/errors.h"
#include "skull/common/panic.h"
#include "skull/eval/types/float.h"
#include "skull/eval/types/int.h"
#include "skull/llvm/aliases.h"
#include "skull/llvm/scope.h"

#include "skull/llvm/oper.h"

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
	if (type == &TYPE_INT) {
		return (Expr){
			.llvm_value = int_func(SKULL_STATE.builder, lhs, rhs, ""),
			.type = type
		};
	}
	if (type == &TYPE_FLOAT) {
		return (Expr){
			.llvm_value = float_func(SKULL_STATE.builder, lhs, rhs, ""),
			.type = type
		};
	}

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
	if (type == &TYPE_INT) {
		return (Expr){
			.llvm_value = LLVMBuildShl(SKULL_STATE.builder, lhs, rhs, ""),
			.type = &TYPE_INT
		};
	}

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
	if (type == &TYPE_INT) {
		return (Expr){
			.llvm_value = LLVMBuildLShr(SKULL_STATE.builder, lhs, rhs, ""),
			.type = &TYPE_INT
		};
	}

	return (Expr){0};
}

/*
Return expression for taking `lhs` to the power of `rhs`.
*/
Expr gen_expr_pow(
	const Type *const type,
	LLVMValueRef lhs,
	LLVMValueRef rhs
) {
	const char *asm_func = NULL;

	if (type == &TYPE_INT) {
		asm_func = ".int_pow";
	}
	else if (type == &TYPE_FLOAT) {
		asm_func = ".float_pow";
	}
	else {
		PANIC(ERR_POW_BAD_TYPE, { .type = type });
	}

	LLVMTypeRef func_type = LLVMFunctionType(
		type->llvm_type(),
		(LLVMTypeRef[]){
			type->llvm_type(),
			type->llvm_type()
		},
		2,
		false
	);

	LLVMValueRef func = LLVMAddFunction(
		SKULL_STATE.module,
		asm_func,
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
		.type = type
	};
}

Expr gen_expr_is_str(LLVMValueRef, LLVMValueRef);

/*
Return expression for result of is operator for `lhs` and `rhs`.
*/
Expr gen_expr_is(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (type == &TYPE_INT || type == &TYPE_RUNE) {
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
	}
	if (type == &TYPE_FLOAT) {
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
	}
	if (type == &TYPE_STR) {
		return gen_expr_is_str(lhs, rhs);
	}

	return (Expr){0};
}

/*
Return expression for string-is operator against `lhs` and `rhs`.
*/
Expr gen_expr_is_str(LLVMValueRef lhs, LLVMValueRef rhs) {
	LLVMTypeRef types[] = {
		TYPE_STR.llvm_type(),
		TYPE_STR.llvm_type()
	};

	LLVMTypeRef func_type = LLVMFunctionType(
		TYPE_BOOL.llvm_type(),
		types,
		2,
		false
	);

	LLVMValueRef func = LLVMAddFunction(
		SKULL_STATE.module,
		".strcmp",
		func_type
	);

	LLVMValueRef values[] = { lhs, rhs };

	return (Expr){
		.llvm_value = LLVMBuildCall2(
			SKULL_STATE.builder,
			func_type,
			func,
			values,
			2,
			""
		),
		.type = &TYPE_BOOL
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
	if (type == &TYPE_INT) {
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
	}
	if (type == &TYPE_FLOAT) {
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
	}

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
Return expression for operation `oper` from `node`.
*/
Expr gen_expr_oper(
	const Type *const type,
	const AstNode *const node,
	Operation *oper
) {
	const Token *rhs_token = ATTR(AstNodeOper, node, rhs);
	const Token *lhs_token = ATTR(AstNodeOper, node, lhs);

	const Expr lhs = token_to_expr(lhs_token, NULL);
	const Expr rhs = token_to_expr(rhs_token, NULL);

	if (lhs.type != rhs.type) {
		PANIC(ERR_TYPE_MISMATCH, {
			.tok = rhs_token,
			.type = lhs.type
		});
	}

	const Expr result = oper(
		lhs.type,
		lhs.llvm_value,
		rhs.llvm_value
	);

	if (!result.type && !result.llvm_value) {
		return (Expr){0};
	}

	if (type && result.type != type) {
		PANIC(ERR_TYPE_MISMATCH, {
			.tok = lhs_token,
			.type = type
		});
	}

	return result;
}
