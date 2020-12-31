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
Build LLVM for assining addition of `lhs` and `rhs`.
*/
Expr llvm_make_add(const Type *type, LLVMValueRef lhs, LLVMValueRef rhs) {
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
Build LLVM for assining subtraction of `lhs` and `rhs`.
*/
Expr llvm_make_sub(const Type *type, LLVMValueRef lhs, LLVMValueRef rhs) {
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
Build LLVM for assining multiplication of `lhs` and `rhs`.
*/
Expr llvm_make_mult(const Type *type, LLVMValueRef lhs, LLVMValueRef rhs) {
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
Build LLVM for assining division of `lhs` and `rhs`.
*/
Expr llvm_make_div(const Type *type, LLVMValueRef lhs, LLVMValueRef rhs) {
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
Build LLVM for assining result of is operator for `lhs` and `rhs`.
*/
LLVMValueRef llvm_make_is(const Type *type, LLVMValueRef lhs, LLVMValueRef rhs) {
	if (type == &TYPE_INT || type == &TYPE_RUNE) {
		return LLVMBuildICmp(
			BUILDER,
			LLVMIntEQ,
			lhs,
			rhs,
			""
		);
	}
	if (type == &TYPE_FLOAT) {
		return LLVMBuildFCmp(
			BUILDER,
			LLVMRealOEQ,
			lhs,
			rhs,
			""
		);
	}

	return NULL;
}

/*
Return LLVM for assigning operation `oper` from `node`.
*/
LLVMValueRef llvm_make_oper(const Type *type, const AstNode *const node, Operation *oper) {
	const Token *rhs_token = ATTR(AstNodeOper, node, rhs);
	const Token *lhs_token = ATTR(AstNodeOper, node, lhs);

	Expr lhs = token_to_expr(lhs_token, NULL);
	Expr rhs = token_to_expr(rhs_token, NULL);

	if (lhs.type != rhs.type) {
		PANIC(ERR_TYPE_MISMATCH, {
			.tok = rhs_token,
			.type = lhs.type
		});
	}

	Expr result = oper(
		lhs.type,
		lhs.llvm_value,
		rhs.llvm_value
	);

	if (!result.type && !result.llvm_value) {
		return NULL;
	}

	if (result.type != type) {
		PANIC(ERR_TYPE_MISMATCH, {
			.tok = lhs_token,
			.type = type
		});
	}

	return result.llvm_value;
}
