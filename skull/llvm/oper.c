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

LLVMValueRef llvm_make_and(LLVMValueRef lhs, LLVMValueRef rhs) {
	return LLVMBuildAnd(
		BUILDER,
		lhs,
		rhs,
		""
	);
}

LLVMValueRef llvm_make_or(LLVMValueRef lhs, LLVMValueRef rhs) {
	return LLVMBuildOr(
		BUILDER,
		lhs,
		rhs,
		""
	);
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
