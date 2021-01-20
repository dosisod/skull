#include <stdbool.h>

#include "skull/common/errors.h"
#include "skull/common/panic.h"
#include "skull/eval/types/float.h"
#include "skull/eval/types/int.h"
#include "skull/llvm/aliases.h"
#include "skull/llvm/scope.h"

#include "skull/llvm/oper.h"

extern LLVMBuilderRef BUILDER;
extern Scope *SCOPE;

/*
Return expression for addition of `lhs` and `rhs`.
*/
Expr llvm_make_add(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (type == &TYPE_INT) {
		return (Expr){
			.llvm_value = LLVMBuildNSWAdd(BUILDER, lhs, rhs, ""),
			.type = type
		};
	}
	if (type == &TYPE_FLOAT) {
		return (Expr){
			.llvm_value = LLVMBuildFAdd(BUILDER, lhs, rhs, ""),
			.type = type
		};
	}

	return (Expr){0};
}

/*
Return expression for subtraction of `lhs` and `rhs`.
*/
Expr llvm_make_sub(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (type == &TYPE_INT) {
		return (Expr){
			.llvm_value = LLVMBuildNSWSub(BUILDER, lhs, rhs, ""),
			.type = type
		};
	}
	if (type == &TYPE_FLOAT) {
		return (Expr){
			.llvm_value = LLVMBuildFSub(BUILDER, lhs, rhs, ""),
			.type = type
		};
	}

	return (Expr){0};
}

/*
Return expression for multiplication of `lhs` and `rhs`.
*/
Expr llvm_make_mult(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (type == &TYPE_INT) {
		return (Expr){
			.llvm_value = LLVMBuildNSWMul(BUILDER, lhs, rhs, ""),
			.type = type
		};
	}
	if (type == &TYPE_FLOAT) {
		return (Expr){
			.llvm_value = LLVMBuildFMul(BUILDER, lhs, rhs, ""),
			.type = type
		};
	}

	return (Expr){0};
}
/*
Return expression for division of `lhs` and `rhs`.
*/
Expr llvm_make_div(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (type == &TYPE_INT) {
		if (LLVMConstIntGetSExtValue(rhs) == 0) {
			PANIC(ERR_DIV_BY_ZERO, {0});
		}

		return (Expr){
			.llvm_value = LLVMBuildExactSDiv(BUILDER, lhs, rhs, ""),
			.type = type
		};
	}
	if (type == &TYPE_FLOAT) {
		return (Expr){
			.llvm_value = LLVMBuildFDiv(BUILDER, lhs, rhs, ""),
			.type = type
		};
	}

	return (Expr){0};
}

/*
Return expression for result of is operator for `lhs` and `rhs`.
*/
Expr llvm_make_is(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (type == &TYPE_INT || type == &TYPE_RUNE) {
		return (Expr){
			.llvm_value = LLVMBuildICmp(
				BUILDER,
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
				BUILDER,
				LLVMRealOEQ,
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
Return expression for result of is not operator for `lhs` and `rhs`.
*/
Expr llvm_make_is_not(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs) {
	Expr expr = llvm_make_is(type, lhs, rhs);

	expr.llvm_value = LLVMBuildNot(
		BUILDER,
		expr.llvm_value,
		""
	);

	return expr;
}

/*
Return expression for result of less than operator for `lhs` and `rhs`.
*/
Expr llvm_make_less_than(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (type == &TYPE_INT) {
		return (Expr){
			.llvm_value = LLVMBuildICmp(
				BUILDER,
				LLVMIntSLT,
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
				BUILDER,
				LLVMRealOLT,
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
Return expression for result of greater than operator for `lhs` and `rhs`.
*/
Expr llvm_make_gtr_than(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (type == &TYPE_INT) {
		return (Expr){
			.llvm_value = LLVMBuildICmp(
				BUILDER,
				LLVMIntSGT,
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
				BUILDER,
				LLVMRealOGT,
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
Return expression for result of less than or equal to operator for `lhs` and `rhs`.
*/
Expr llvm_make_less_than_eq(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (type == &TYPE_INT) {
		return (Expr){
			.llvm_value = LLVMBuildICmp(
				BUILDER,
				LLVMIntSLE,
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
				BUILDER,
				LLVMRealOLE,
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
Return expression for result of greater than or equal to operator for `lhs` and `rhs`.
*/
Expr llvm_make_gtr_than_eq(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (type == &TYPE_INT) {
		return (Expr){
			.llvm_value = LLVMBuildICmp(
				BUILDER,
				LLVMIntSGE,
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
				BUILDER,
				LLVMRealOGE,
				lhs,
				rhs,
				""
			),
			.type = &TYPE_BOOL
		};
	}

	return (Expr){0};
}

Expr llvm_make_and(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs) {
	return (Expr){
		.llvm_value = LLVMBuildAnd(
			BUILDER,
			lhs,
			rhs,
			""
		),
		.type = type
	};
}

Expr llvm_make_or(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs) {
	return (Expr){
		.llvm_value = LLVMBuildOr(
			BUILDER,
			lhs,
			rhs,
			""
		),
		.type = type
	};
}

Expr llvm_make_xor(const Type *const type, LLVMValueRef lhs, LLVMValueRef rhs) {
	return (Expr){
		.llvm_value = LLVMBuildXor(
			BUILDER,
			lhs,
			rhs,
			""
		),
		.type = type
	};
}

/*
Return expression for operation `oper` from `node`.
*/
Expr llvm_make_oper(const Type *const type, const AstNode *const node, Operation *oper) {
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
